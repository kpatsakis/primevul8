static UINT serial_process_irp(SERIAL_DEVICE* serial, IRP* irp)
{
	UINT error = CHANNEL_RC_OK;
	WLog_Print(serial->log, WLOG_DEBUG,
	           "IRP MajorFunction: 0x%08" PRIX32 " MinorFunction: 0x%08" PRIX32 "\n",
	           irp->MajorFunction, irp->MinorFunction);

	switch (irp->MajorFunction)
	{
		case IRP_MJ_CREATE:
			error = serial_process_irp_create(serial, irp);
			break;

		case IRP_MJ_CLOSE:
			error = serial_process_irp_close(serial, irp);
			break;

		case IRP_MJ_READ:
			if ((error = serial_process_irp_read(serial, irp)))
				WLog_ERR(TAG, "serial_process_irp_read failed with error %" PRIu32 "!", error);

			break;

		case IRP_MJ_WRITE:
			error = serial_process_irp_write(serial, irp);
			break;

		case IRP_MJ_DEVICE_CONTROL:
			if ((error = serial_process_irp_device_control(serial, irp)))
				WLog_ERR(TAG, "serial_process_irp_device_control failed with error %" PRIu32 "!",
				         error);

			break;

		default:
			irp->IoStatus = STATUS_NOT_SUPPORTED;
			break;
	}

	return error;
}