static UINT drive_process_irp_device_control(DRIVE_DEVICE* drive, IRP* irp)
{
	if (!drive || !irp)
		return ERROR_INVALID_PARAMETER;

	Stream_Write_UINT32(irp->output, 0); /* OutputBufferLength */
	return irp->Complete(irp);
}