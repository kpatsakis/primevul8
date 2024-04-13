static UINT drive_process_irp_silent_ignore(DRIVE_DEVICE* drive, IRP* irp)
{
	UINT32 FsInformationClass;

	if (!drive || !irp || !irp->output || !irp->Complete)
		return ERROR_INVALID_PARAMETER;

	if (Stream_GetRemainingLength(irp->input) < 4)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, FsInformationClass);
	Stream_Write_UINT32(irp->output, 0); /* Length */
	return irp->Complete(irp);
}