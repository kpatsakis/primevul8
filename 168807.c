UINT DeviceServiceEntry(PDEVICE_SERVICE_ENTRY_POINTS pEntryPoints)
{
	RDPDR_DRIVE* drive;
	UINT error;
#ifdef WIN32
	char* dev;
	int len;
	char devlist[512], buf[512];
	char* bufdup;
	char* devdup;
#endif
	drive = (RDPDR_DRIVE*)pEntryPoints->device;
#ifndef WIN32
	sys_code_page = CP_UTF8;

	if (strcmp(drive->Path, "*") == 0)
	{
		/* all drives */
		free(drive->Path);
		drive->Path = _strdup("/");

		if (!drive->Path)
		{
			WLog_ERR(TAG, "_strdup failed!");
			return CHANNEL_RC_NO_MEMORY;
		}
	}
	else if (strcmp(drive->Path, "%") == 0)
	{
		free(drive->Path);
		drive->Path = GetKnownPath(KNOWN_PATH_HOME);

		if (!drive->Path)
		{
			WLog_ERR(TAG, "_strdup failed!");
			return CHANNEL_RC_NO_MEMORY;
		}
	}

	error = drive_register_drive_path(pEntryPoints, drive->Name, drive->Path, drive->automount);
#else
	sys_code_page = GetACP();

	/* Special case: path[0] == '*' -> export all drives */
	/* Special case: path[0] == '%' -> user home dir */
	if (strcmp(drive->Path, "%") == 0)
	{
		GetEnvironmentVariableA("USERPROFILE", buf, sizeof(buf));
		PathCchAddBackslashA(buf, sizeof(buf));
		free(drive->Path);
		drive->Path = _strdup(buf);

		if (!drive->Path)
		{
			WLog_ERR(TAG, "_strdup failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		error = drive_register_drive_path(pEntryPoints, drive->Name, drive->Path, drive->automount);
	}
	else if (strcmp(drive->Path, "*") == 0)
	{
		int i;
		/* Enumerate all devices: */
		GetLogicalDriveStringsA(sizeof(devlist) - 1, devlist);

		for (dev = devlist, i = 0; *dev; dev += 4, i++)
		{
			if (*dev > 'B')
			{
				/* Suppress disk drives A and B to avoid pesty messages */
				len = sprintf_s(buf, sizeof(buf) - 4, "%s", drive->Name);
				buf[len] = '_';
				buf[len + 1] = dev[0];
				buf[len + 2] = 0;
				buf[len + 3] = 0;

				if (!(bufdup = _strdup(buf)))
				{
					WLog_ERR(TAG, "_strdup failed!");
					return CHANNEL_RC_NO_MEMORY;
				}

				if (!(devdup = _strdup(dev)))
				{
					WLog_ERR(TAG, "_strdup failed!");
					return CHANNEL_RC_NO_MEMORY;
				}

				if ((error = drive_register_drive_path(pEntryPoints, bufdup, devdup, TRUE)))
				{
					break;
				}
			}
		}
	}
	else
	{
		error = drive_register_drive_path(pEntryPoints, drive->Name, drive->Path, drive->automount);
	}

#endif
	return error;
}