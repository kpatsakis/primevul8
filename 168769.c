static UINT serial_irp_request(DEVICE* device, IRP* irp)
{
	SERIAL_DEVICE* serial = (SERIAL_DEVICE*)device;
	assert(irp != NULL);

	if (irp == NULL)
		return CHANNEL_RC_OK;

	/* NB: ENABLE_ASYNCIO is set, (MS-RDPEFS 2.2.2.7.2) this
	 * allows the server to send multiple simultaneous read or
	 * write requests.
	 */

	if (!MessageQueue_Post(serial->MainIrpQueue, NULL, 0, (void*)irp, NULL))
	{
		WLog_ERR(TAG, "MessageQueue_Post failed!");
		return ERROR_INTERNAL_ERROR;
	}

	return CHANNEL_RC_OK;
}