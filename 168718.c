static UINT drive_register_drive_path(PDEVICE_SERVICE_ENTRY_POINTS pEntryPoints, const char* name,
                                      const char* path, BOOL automount)
{
	size_t i, length;
	DRIVE_DEVICE* drive;
	UINT error = ERROR_INTERNAL_ERROR;

	if (!pEntryPoints || !name || !path)
	{
		WLog_ERR(TAG, "[%s] Invalid parameters: pEntryPoints=%p, name=%p, path=%p", pEntryPoints,
		         name, path);
		return ERROR_INVALID_PARAMETER;
	}

	if (name[0] && path[0])
	{
		size_t pathLength = strnlen(path, MAX_PATH);
		drive = (DRIVE_DEVICE*)calloc(1, sizeof(DRIVE_DEVICE));

		if (!drive)
		{
			WLog_ERR(TAG, "calloc failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		drive->device.type = RDPDR_DTYP_FILESYSTEM;
		drive->device.IRPRequest = drive_irp_request;
		drive->device.Free = drive_free;
		drive->rdpcontext = pEntryPoints->rdpcontext;
		drive->automount = automount;
		length = strlen(name);
		drive->device.data = Stream_New(NULL, length + 1);

		if (!drive->device.data)
		{
			WLog_ERR(TAG, "Stream_New failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto out_error;
		}

		for (i = 0; i < length; i++)
		{
			/* Filter 2.2.1.3 Device Announce Header (DEVICE_ANNOUNCE) forbidden symbols */
			switch (name[i])
			{
				case ':':
				case '<':
				case '>':
				case '\"':
				case '/':
				case '\\':
				case '|':
				case ' ':
					Stream_Write_UINT8(drive->device.data, '_');
					break;
				default:
					Stream_Write_UINT8(drive->device.data, (BYTE)name[i]);
					break;
			}
		}
		Stream_Write_UINT8(drive->device.data, '\0');

		drive->device.name = (const char*)Stream_Buffer(drive->device.data);
		if (!drive->device.name)
			goto out_error;

		if ((pathLength > 1) && (path[pathLength - 1] == '/'))
			pathLength--;

		if (ConvertToUnicode(sys_code_page, 0, path, pathLength, &drive->path, 0) <= 0)
		{
			WLog_ERR(TAG, "ConvertToUnicode failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto out_error;
		}

		drive->files = ListDictionary_New(TRUE);

		if (!drive->files)
		{
			WLog_ERR(TAG, "ListDictionary_New failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto out_error;
		}

		ListDictionary_ValueObject(drive->files)->fnObjectFree = drive_file_objfree;
		drive->IrpQueue = MessageQueue_New(NULL);

		if (!drive->IrpQueue)
		{
			WLog_ERR(TAG, "ListDictionary_New failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto out_error;
		}

		if ((error = pEntryPoints->RegisterDevice(pEntryPoints->devman, (DEVICE*)drive)))
		{
			WLog_ERR(TAG, "RegisterDevice failed with error %" PRIu32 "!", error);
			goto out_error;
		}

		if (!(drive->thread =
		          CreateThread(NULL, 0, drive_thread_func, drive, CREATE_SUSPENDED, NULL)))
		{
			WLog_ERR(TAG, "CreateThread failed!");
			goto out_error;
		}

		ResumeThread(drive->thread);
	}

	return CHANNEL_RC_OK;
out_error:
	drive_free_int(drive);
	return error;
}