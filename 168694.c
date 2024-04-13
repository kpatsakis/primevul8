static UINT drive_process_irp_create(DRIVE_DEVICE* drive, IRP* irp)
{
	UINT32 FileId;
	DRIVE_FILE* file;
	BYTE Information;
	UINT32 FileAttributes;
	UINT32 SharedAccess;
	UINT32 DesiredAccess;
	UINT32 CreateDisposition;
	UINT32 CreateOptions;
	UINT32 PathLength;
	UINT64 allocationSize;
	const WCHAR* path;

	if (!drive || !irp || !irp->devman || !irp->Complete)
		return ERROR_INVALID_PARAMETER;

	if (Stream_GetRemainingLength(irp->input) < 6 * 4 + 8)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, DesiredAccess);
	Stream_Read_UINT64(irp->input, allocationSize);
	Stream_Read_UINT32(irp->input, FileAttributes);
	Stream_Read_UINT32(irp->input, SharedAccess);
	Stream_Read_UINT32(irp->input, CreateDisposition);
	Stream_Read_UINT32(irp->input, CreateOptions);
	Stream_Read_UINT32(irp->input, PathLength);

	if (Stream_GetRemainingLength(irp->input) < PathLength)
		return ERROR_INVALID_DATA;

	path = (const WCHAR*)Stream_Pointer(irp->input);
	FileId = irp->devman->id_sequence++;
	file = drive_file_new(drive->path, path, PathLength, FileId, DesiredAccess, CreateDisposition,
	                      CreateOptions, FileAttributes, SharedAccess);

	if (!file)
	{
		irp->IoStatus = drive_map_windows_err(GetLastError());
		FileId = 0;
		Information = 0;
	}
	else
	{
		void* key = (void*)(size_t)file->id;

		if (!ListDictionary_Add(drive->files, key, file))
		{
			WLog_ERR(TAG, "ListDictionary_Add failed!");
			return ERROR_INTERNAL_ERROR;
		}

		switch (CreateDisposition)
		{
			case FILE_SUPERSEDE:
			case FILE_OPEN:
			case FILE_CREATE:
			case FILE_OVERWRITE:
				Information = FILE_SUPERSEDED;
				break;

			case FILE_OPEN_IF:
				Information = FILE_OPENED;
				break;

			case FILE_OVERWRITE_IF:
				Information = FILE_OVERWRITTEN;
				break;

			default:
				Information = 0;
				break;
		}
	}

	Stream_Write_UINT32(irp->output, FileId);
	Stream_Write_UINT8(irp->output, Information);
	return irp->Complete(irp);
}