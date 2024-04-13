static UINT drive_process_irp_directory_control(DRIVE_DEVICE* drive, IRP* irp)
{
	if (!drive || !irp)
		return ERROR_INVALID_PARAMETER;

	switch (irp->MinorFunction)
	{
		case IRP_MN_QUERY_DIRECTORY:
			return drive_process_irp_query_directory(drive, irp);

		case IRP_MN_NOTIFY_CHANGE_DIRECTORY: /* TODO */
			return irp->Discard(irp);

		default:
			irp->IoStatus = STATUS_NOT_SUPPORTED;
			Stream_Write_UINT32(irp->output, 0); /* Length */
			return irp->Complete(irp);
	}

	return CHANNEL_RC_OK;
}