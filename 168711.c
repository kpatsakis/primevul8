static UINT drive_process_irp(DRIVE_DEVICE* drive, IRP* irp)
{
	UINT error;

	if (!drive || !irp)
		return ERROR_INVALID_PARAMETER;

	irp->IoStatus = STATUS_SUCCESS;

	switch (irp->MajorFunction)
	{
		case IRP_MJ_CREATE:
			error = drive_process_irp_create(drive, irp);
			break;

		case IRP_MJ_CLOSE:
			error = drive_process_irp_close(drive, irp);
			break;

		case IRP_MJ_READ:
			error = drive_process_irp_read(drive, irp);
			break;

		case IRP_MJ_WRITE:
			error = drive_process_irp_write(drive, irp);
			break;

		case IRP_MJ_QUERY_INFORMATION:
			error = drive_process_irp_query_information(drive, irp);
			break;

		case IRP_MJ_SET_INFORMATION:
			error = drive_process_irp_set_information(drive, irp);
			break;

		case IRP_MJ_QUERY_VOLUME_INFORMATION:
			error = drive_process_irp_query_volume_information(drive, irp);
			break;

		case IRP_MJ_LOCK_CONTROL:
			error = drive_process_irp_silent_ignore(drive, irp);
			break;

		case IRP_MJ_DIRECTORY_CONTROL:
			error = drive_process_irp_directory_control(drive, irp);
			break;

		case IRP_MJ_DEVICE_CONTROL:
			error = drive_process_irp_device_control(drive, irp);
			break;

		default:
			irp->IoStatus = STATUS_NOT_SUPPORTED;
			error = irp->Complete(irp);
			break;
	}

	return error;
}