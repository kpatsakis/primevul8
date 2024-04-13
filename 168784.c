static UINT drive_process_irp_query_information(DRIVE_DEVICE* drive, IRP* irp)
{
	DRIVE_FILE* file;
	UINT32 FsInformationClass;

	if (!drive || !irp || !irp->Complete)
		return ERROR_INVALID_PARAMETER;

	if (Stream_GetRemainingLength(irp->input) < 4)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, FsInformationClass);
	file = drive_get_file_by_id(drive, irp->FileId);

	if (!file)
	{
		irp->IoStatus = STATUS_UNSUCCESSFUL;
	}
	else if (!drive_file_query_information(file, FsInformationClass, irp->output))
	{
		irp->IoStatus = drive_map_windows_err(GetLastError());
	}

	return irp->Complete(irp);
}