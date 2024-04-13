static UINT printer_register(PDEVICE_SERVICE_ENTRY_POINTS pEntryPoints, rdpPrinter* printer)
{
	PRINTER_DEVICE* printer_dev;
	UINT error = ERROR_INTERNAL_ERROR;
	printer_dev = (PRINTER_DEVICE*)calloc(1, sizeof(PRINTER_DEVICE));

	if (!printer_dev)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	printer_dev->device.data = Stream_New(NULL, 1024);

	if (!printer_dev->device.data)
		goto error_out;

	sprintf_s(printer_dev->port, sizeof(printer_dev->port), "PRN%" PRIdz, printer->id);
	printer_dev->device.type = RDPDR_DTYP_PRINT;
	printer_dev->device.name = printer_dev->port;
	printer_dev->device.IRPRequest = printer_irp_request;
	printer_dev->device.CustomComponentRequest = printer_custom_component;
	printer_dev->device.Free = printer_free;
	printer_dev->rdpcontext = pEntryPoints->rdpcontext;
	printer_dev->printer = printer;
	printer_dev->pIrpList = (WINPR_PSLIST_HEADER)_aligned_malloc(sizeof(WINPR_SLIST_HEADER),
	                                                             MEMORY_ALLOCATION_ALIGNMENT);

	if (!printer_dev->pIrpList)
	{
		WLog_ERR(TAG, "_aligned_malloc failed!");
		error = CHANNEL_RC_NO_MEMORY;
		goto error_out;
	}

	if (!printer_load_from_config(pEntryPoints->rdpcontext->settings, printer, printer_dev))
		goto error_out;

	InitializeSListHead(printer_dev->pIrpList);

	if (!(printer_dev->event = CreateEvent(NULL, TRUE, FALSE, NULL)))
	{
		WLog_ERR(TAG, "CreateEvent failed!");
		error = ERROR_INTERNAL_ERROR;
		goto error_out;
	}

	if (!(printer_dev->stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
	{
		WLog_ERR(TAG, "CreateEvent failed!");
		error = ERROR_INTERNAL_ERROR;
		goto error_out;
	}

	if ((error = pEntryPoints->RegisterDevice(pEntryPoints->devman, (DEVICE*)printer_dev)))
	{
		WLog_ERR(TAG, "RegisterDevice failed with error %" PRIu32 "!", error);
		goto error_out;
	}

	if (!(printer_dev->thread =
	          CreateThread(NULL, 0, printer_thread_func, (void*)printer_dev, 0, NULL)))
	{
		WLog_ERR(TAG, "CreateThread failed!");
		error = ERROR_INTERNAL_ERROR;
		goto error_out;
	}

	printer->AddRef(printer);
	return CHANNEL_RC_OK;
error_out:
	printer_free(&printer_dev->device);
	return error;
}