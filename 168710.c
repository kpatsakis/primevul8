UINT DeviceServiceEntry(PDEVICE_SERVICE_ENTRY_POINTS pEntryPoints)
{
	char* name;
	char* path;
	char* driver;
	RDPDR_SERIAL* device;
#if defined __linux__ && !defined ANDROID
	size_t i, len;
	SERIAL_DEVICE* serial;
#endif /* __linux__ */
	UINT error = CHANNEL_RC_OK;
	device = (RDPDR_SERIAL*)pEntryPoints->device;
	name = device->Name;
	path = device->Path;
	driver = device->Driver;

	if (!name || (name[0] == '*'))
	{
		/* TODO: implement auto detection of serial ports */
		return CHANNEL_RC_OK;
	}

	if ((name && name[0]) && (path && path[0]))
	{
		wLog* log;
		log = WLog_Get("com.freerdp.channel.serial.client");
		WLog_Print(log, WLOG_DEBUG, "initializing");
#ifndef __linux__ /* to be removed */
		WLog_Print(log, WLOG_WARN, "Serial ports redirection not supported on this platform.");
		return CHANNEL_RC_INITIALIZATION_ERROR;
#else /* __linux __ */
		WLog_Print(log, WLOG_DEBUG, "Defining %s as %s", name, path);

		if (!DefineCommDevice(name /* eg: COM1 */, path /* eg: /dev/ttyS0 */))
		{
			DWORD status = GetLastError();
			WLog_ERR(TAG, "DefineCommDevice failed with %08" PRIx32, status);
			return ERROR_INTERNAL_ERROR;
		}

		serial = (SERIAL_DEVICE*)calloc(1, sizeof(SERIAL_DEVICE));

		if (!serial)
		{
			WLog_ERR(TAG, "calloc failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		serial->log = log;
		serial->device.type = RDPDR_DTYP_SERIAL;
		serial->device.name = name;
		serial->device.IRPRequest = serial_irp_request;
		serial->device.Free = serial_free;
		serial->rdpcontext = pEntryPoints->rdpcontext;
		len = strlen(name);
		serial->device.data = Stream_New(NULL, len + 1);

		if (!serial->device.data)
		{
			WLog_ERR(TAG, "calloc failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto error_out;
		}

		for (i = 0; i <= len; i++)
			Stream_Write_UINT8(serial->device.data, name[i] < 0 ? '_' : name[i]);

		if (driver != NULL)
		{
			if (_stricmp(driver, "Serial") == 0)
				serial->ServerSerialDriverId = SerialDriverSerialSys;
			else if (_stricmp(driver, "SerCx") == 0)
				serial->ServerSerialDriverId = SerialDriverSerCxSys;
			else if (_stricmp(driver, "SerCx2") == 0)
				serial->ServerSerialDriverId = SerialDriverSerCx2Sys;
			else
			{
				assert(FALSE);
				WLog_Print(serial->log, WLOG_DEBUG,
				           "Unknown server's serial driver: %s. SerCx2 will be used", driver);
				serial->ServerSerialDriverId = SerialDriverSerialSys;
			}
		}
		else
		{
			/* default driver */
			serial->ServerSerialDriverId = SerialDriverSerialSys;
		}

		if (device->Permissive != NULL)
		{
			if (_stricmp(device->Permissive, "permissive") == 0)
			{
				serial->permissive = TRUE;
			}
			else
			{
				WLog_Print(serial->log, WLOG_DEBUG, "Unknown flag: %s", device->Permissive);
				assert(FALSE);
			}
		}

		WLog_Print(serial->log, WLOG_DEBUG, "Server's serial driver: %s (id: %d)", driver,
		           serial->ServerSerialDriverId);
		/* TODO: implement auto detection of the server's serial driver */
		serial->MainIrpQueue = MessageQueue_New(NULL);

		if (!serial->MainIrpQueue)
		{
			WLog_ERR(TAG, "MessageQueue_New failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto error_out;
		}

		/* IrpThreads content only modified by create_irp_thread() */
		serial->IrpThreads = ListDictionary_New(FALSE);

		if (!serial->IrpThreads)
		{
			WLog_ERR(TAG, "ListDictionary_New failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto error_out;
		}

		serial->IrpThreadToBeTerminatedCount = 0;
		InitializeCriticalSection(&serial->TerminatingIrpThreadsLock);

		if ((error = pEntryPoints->RegisterDevice(pEntryPoints->devman, (DEVICE*)serial)))
		{
			WLog_ERR(TAG, "EntryPoints->RegisterDevice failed with error %" PRIu32 "!", error);
			goto error_out;
		}

		if (!(serial->MainThread =
		          CreateThread(NULL, 0, serial_thread_func, (void*)serial, 0, NULL)))
		{
			WLog_ERR(TAG, "CreateThread failed!");
			error = ERROR_INTERNAL_ERROR;
			goto error_out;
		}

#endif /* __linux __ */
	}

	return error;
error_out:
#ifdef __linux__ /* to be removed */
	ListDictionary_Free(serial->IrpThreads);
	MessageQueue_Free(serial->MainIrpQueue);
	Stream_Free(serial->device.data, TRUE);
	free(serial);
#endif /* __linux __ */
	return error;
}