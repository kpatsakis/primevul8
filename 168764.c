static UINT drive_free_int(DRIVE_DEVICE* drive)
{
	UINT error = CHANNEL_RC_OK;

	if (!drive)
		return ERROR_INVALID_PARAMETER;

	CloseHandle(drive->thread);
	ListDictionary_Free(drive->files);
	MessageQueue_Free(drive->IrpQueue);
	Stream_Free(drive->device.data, TRUE);
	free(drive->path);
	free(drive);
	return error;
}