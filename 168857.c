static UINT drive_process_irp_close(DRIVE_DEVICE* drive, IRP* irp)
{
	void* key;
	DRIVE_FILE* file;

	if (!drive || !irp || !irp->Complete || !irp->output)
		return ERROR_INVALID_PARAMETER;

	file = drive_get_file_by_id(drive, irp->FileId);
	key = (void*)(size_t)irp->FileId;

	if (!file)
		irp->IoStatus = STATUS_UNSUCCESSFUL;
	else
	{
		ListDictionary_Remove(drive->files, key);

		if (drive_file_free(file))
			irp->IoStatus = STATUS_SUCCESS;
		else
			irp->IoStatus = drive_map_windows_err(GetLastError());
	}

	Stream_Zero(irp->output, 5); /* Padding(5) */
	return irp->Complete(irp);
}