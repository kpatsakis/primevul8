static UINT drive_free(DEVICE* device)
{
	DRIVE_DEVICE* drive = (DRIVE_DEVICE*)device;
	UINT error = CHANNEL_RC_OK;

	if (!drive)
		return ERROR_INVALID_PARAMETER;

	if (MessageQueue_PostQuit(drive->IrpQueue, 0) &&
	    (WaitForSingleObject(drive->thread, INFINITE) == WAIT_FAILED))
	{
		error = GetLastError();
		WLog_ERR(TAG, "WaitForSingleObject failed with error %" PRIu32 "", error);
		return error;
	}

	return drive_free_int(drive);
}