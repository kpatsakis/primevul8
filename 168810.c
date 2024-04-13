static DWORD WINAPI drive_thread_func(LPVOID arg)
{
	IRP* irp;
	wMessage message;
	DRIVE_DEVICE* drive = (DRIVE_DEVICE*)arg;
	UINT error = CHANNEL_RC_OK;

	if (!drive)
	{
		error = ERROR_INVALID_PARAMETER;
		goto fail;
	}

	while (1)
	{
		if (!MessageQueue_Wait(drive->IrpQueue))
		{
			WLog_ERR(TAG, "MessageQueue_Wait failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if (!MessageQueue_Peek(drive->IrpQueue, &message, TRUE))
		{
			WLog_ERR(TAG, "MessageQueue_Peek failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if (message.id == WMQ_QUIT)
			break;

		irp = (IRP*)message.wParam;

		if (irp)
		{
			if ((error = drive_process_irp(drive, irp)))
			{
				WLog_ERR(TAG, "drive_process_irp failed with error %" PRIu32 "!", error);
				break;
			}
		}
	}

fail:

	if (error && drive && drive->rdpcontext)
		setChannelError(drive->rdpcontext, error, "drive_thread_func reported an error");

	ExitThread(error);
	return error;
}