static UINT drive_process_irp_set_information(DRIVE_DEVICE* drive, IRP* irp)
{
	DRIVE_FILE* file;
	UINT32 FsInformationClass;
	UINT32 Length;

	if (!drive || !irp || !irp->Complete || !irp->input || !irp->output)
		return ERROR_INVALID_PARAMETER;

	if (Stream_GetRemainingLength(irp->input) < 32)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, FsInformationClass);
	Stream_Read_UINT32(irp->input, Length);
	Stream_Seek(irp->input, 24); /* Padding */
	file = drive_get_file_by_id(drive, irp->FileId);

	if (!file)
	{
		irp->IoStatus = STATUS_UNSUCCESSFUL;
	}
	else if (!drive_file_set_information(file, FsInformationClass, Length, irp->input))
	{
		irp->IoStatus = drive_map_windows_err(GetLastError());
	}

	if (file && file->is_dir && !PathIsDirectoryEmptyW(file->fullpath))
		irp->IoStatus = STATUS_DIRECTORY_NOT_EMPTY;

	Stream_Write_UINT32(irp->output, Length);
	return irp->Complete(irp);
}