static UINT drive_process_irp_read(DRIVE_DEVICE* drive, IRP* irp)
{
	DRIVE_FILE* file;
	UINT32 Length;
	UINT64 Offset;

	if (!drive || !irp || !irp->output || !irp->Complete)
		return ERROR_INVALID_PARAMETER;

	if (Stream_GetRemainingLength(irp->input) < 12)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, Length);
	Stream_Read_UINT64(irp->input, Offset);
	file = drive_get_file_by_id(drive, irp->FileId);

	if (!file)
	{
		irp->IoStatus = STATUS_UNSUCCESSFUL;
		Length = 0;
	}
	else if (!drive_file_seek(file, Offset))
	{
		irp->IoStatus = drive_map_windows_err(GetLastError());
		Length = 0;
	}

	if (!Stream_EnsureRemainingCapacity(irp->output, Length + 4))
	{
		WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
		return ERROR_INTERNAL_ERROR;
	}
	else if (Length == 0)
		Stream_Write_UINT32(irp->output, 0);
	else
	{
		BYTE* buffer = Stream_Pointer(irp->output) + sizeof(UINT32);

		if (!drive_file_read(file, buffer, &Length))
		{
			irp->IoStatus = drive_map_windows_err(GetLastError());
			Stream_Write_UINT32(irp->output, 0);
		}
		else
		{
			Stream_Write_UINT32(irp->output, Length);
			Stream_Seek(irp->output, Length);
		}
	}

	return irp->Complete(irp);
}