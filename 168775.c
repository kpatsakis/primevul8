static UINT printer_process_irp(PRINTER_DEVICE* printer_dev, IRP* irp)
{
	UINT error;

	switch (irp->MajorFunction)
	{
		case IRP_MJ_CREATE:
			if ((error = printer_process_irp_create(printer_dev, irp)))
			{
				WLog_ERR(TAG, "printer_process_irp_create failed with error %" PRIu32 "!", error);
				return error;
			}

			break;

		case IRP_MJ_CLOSE:
			if ((error = printer_process_irp_close(printer_dev, irp)))
			{
				WLog_ERR(TAG, "printer_process_irp_close failed with error %" PRIu32 "!", error);
				return error;
			}

			break;

		case IRP_MJ_WRITE:
			if ((error = printer_process_irp_write(printer_dev, irp)))
			{
				WLog_ERR(TAG, "printer_process_irp_write failed with error %" PRIu32 "!", error);
				return error;
			}

			break;

		case IRP_MJ_DEVICE_CONTROL:
			if ((error = printer_process_irp_device_control(printer_dev, irp)))
			{
				WLog_ERR(TAG, "printer_process_irp_device_control failed with error %" PRIu32 "!",
				         error);
				return error;
			}

			break;

		default:
			irp->IoStatus = STATUS_NOT_SUPPORTED;
			return irp->Complete(irp);
			break;
	}

	return CHANNEL_RC_OK;
}