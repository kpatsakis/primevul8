static DWORD WINAPI serial_thread_func(LPVOID arg)
{
	IRP* irp;
	wMessage message;
	SERIAL_DEVICE* serial = (SERIAL_DEVICE*)arg;
	UINT error = CHANNEL_RC_OK;

	while (1)
	{
		if (!MessageQueue_Wait(serial->MainIrpQueue))
		{
			WLog_ERR(TAG, "MessageQueue_Wait failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if (!MessageQueue_Peek(serial->MainIrpQueue, &message, TRUE))
		{
			WLog_ERR(TAG, "MessageQueue_Peek failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if (message.id == WMQ_QUIT)
		{
			terminate_pending_irp_threads(serial);
			break;
		}

		irp = (IRP*)message.wParam;

		if (irp)
			create_irp_thread(serial, irp);
	}

	if (error && serial->rdpcontext)
		setChannelError(serial->rdpcontext, error, "serial_thread_func reported an error");

	ExitThread(error);
	return error;
}