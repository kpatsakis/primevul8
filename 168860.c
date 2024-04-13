static UINT drive_irp_request(DEVICE* device, IRP* irp)
{
	DRIVE_DEVICE* drive = (DRIVE_DEVICE*)device;

	if (!drive)
		return ERROR_INVALID_PARAMETER;

	if (!MessageQueue_Post(drive->IrpQueue, NULL, 0, (void*)irp, NULL))
	{
		WLog_ERR(TAG, "MessageQueue_Post failed!");
		return ERROR_INTERNAL_ERROR;
	}

	return CHANNEL_RC_OK;
}