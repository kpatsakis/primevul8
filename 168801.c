    DeviceServiceEntry
#endif
    (PDEVICE_SERVICE_ENTRY_POINTS pEntryPoints)
{
	int i;
	char* name;
	char* driver_name;
	BOOL default_backend = TRUE;
	RDPDR_PRINTER* device = NULL;
	rdpPrinterDriver* driver = NULL;
	UINT error = CHANNEL_RC_OK;

	if (!pEntryPoints || !pEntryPoints->device)
		return ERROR_INVALID_PARAMETER;

	device = (RDPDR_PRINTER*)pEntryPoints->device;
	name = device->Name;
	driver_name = device->DriverName;

	/* Secondary argument is one of the following:
	 *
	 * <driver_name>                ... name of a printer driver
	 * <driver_name>:<backend_name> ... name of a printer driver and local printer backend to use
	 */
	if (driver_name)
	{
		char* sep = strstr(driver_name, ":");
		if (sep)
		{
			const char* backend = sep + 1;
			*sep = '\0';
			driver = printer_load_backend(backend);
			default_backend = FALSE;
		}
	}

	if (!driver && default_backend)
	{
		const char* backend =
#if defined(WITH_CUPS)
		    "cups"
#elif defined(_WIN32)
		    "win"
#else
		    ""
#endif
		    ;

		driver = printer_load_backend(backend);
	}

	if (!driver)
	{
		WLog_ERR(TAG, "Could not get a printer driver!");
		return CHANNEL_RC_INITIALIZATION_ERROR;
	}

	if (name && name[0])
	{
		rdpPrinter* printer = driver->GetPrinter(driver, name, driver_name);

		if (!printer)
		{
			WLog_ERR(TAG, "Could not get printer %s!", name);
			error = CHANNEL_RC_INITIALIZATION_ERROR;
			goto fail;
		}

		if (!printer_save_default_config(pEntryPoints->rdpcontext->settings, printer))
		{
			error = CHANNEL_RC_INITIALIZATION_ERROR;
			printer->ReleaseRef(printer);
			goto fail;
		}

		if ((error = printer_register(pEntryPoints, printer)))
		{
			WLog_ERR(TAG, "printer_register failed with error %" PRIu32 "!", error);
			printer->ReleaseRef(printer);
			goto fail;
		}
	}
	else
	{
		rdpPrinter** printers = driver->EnumPrinters(driver);
		rdpPrinter** current = printers;

		for (i = 0; current[i]; i++)
		{
			rdpPrinter* printer = current[i];

			if ((error = printer_register(pEntryPoints, printer)))
			{
				WLog_ERR(TAG, "printer_register failed with error %" PRIu32 "!", error);
				break;
			}
		}

		driver->ReleaseEnumPrinters(printers);
	}

fail:
	driver->ReleaseRef(driver);

	return error;
}