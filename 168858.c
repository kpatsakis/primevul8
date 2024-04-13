static UINT drive_process_irp_query_directory(DRIVE_DEVICE* drive, IRP* irp)
{
	const WCHAR* path;
	DRIVE_FILE* file;
	BYTE InitialQuery;
	UINT32 PathLength;
	UINT32 FsInformationClass;

	if (!drive || !irp || !irp->Complete)
		return ERROR_INVALID_PARAMETER;

	if (Stream_GetRemainingLength(irp->input) < 32)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, FsInformationClass);
	Stream_Read_UINT8(irp->input, InitialQuery);
	Stream_Read_UINT32(irp->input, PathLength);
	Stream_Seek(irp->input, 23); /* Padding */
	path = (WCHAR*)Stream_Pointer(irp->input);
	file = drive_get_file_by_id(drive, irp->FileId);

	if (file == NULL)
	{
		irp->IoStatus = STATUS_UNSUCCESSFUL;
		Stream_Write_UINT32(irp->output, 0); /* Length */
	}
	else if (!drive_file_query_directory(file, FsInformationClass, InitialQuery, path, PathLength,
	                                     irp->output))
	{
		irp->IoStatus = drive_map_windows_err(GetLastError());
	}

	return irp->Complete(irp);
}