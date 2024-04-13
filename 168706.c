static DWORD WINAPI printer_thread_func(LPVOID arg)
{
	IRP* irp;
	PRINTER_DEVICE* printer_dev = (PRINTER_DEVICE*)arg;
	HANDLE obj[] = { printer_dev->event, printer_dev->stopEvent };
	UINT error = CHANNEL_RC_OK;

	while (1)
	{
		DWORD rc = WaitForMultipleObjects(2, obj, FALSE, INFINITE);

		if (rc == WAIT_FAILED)
		{
			error = GetLastError();
			WLog_ERR(TAG, "WaitForMultipleObjects failed with error %" PRIu32 "!", error);
			break;
		}

		if (rc == WAIT_OBJECT_0 + 1)
			break;
		else if (rc != WAIT_OBJECT_0)
			continue;

		ResetEvent(printer_dev->event);
		irp = (IRP*)InterlockedPopEntrySList(printer_dev->pIrpList);

		if (irp == NULL)
		{
			WLog_ERR(TAG, "InterlockedPopEntrySList failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if ((error = printer_process_irp(printer_dev, irp)))
		{
			WLog_ERR(TAG, "printer_process_irp failed with error %" PRIu32 "!", error);
			break;
		}
	}

	if (error && printer_dev->rdpcontext)
		setChannelError(printer_dev->rdpcontext, error, "printer_thread_func reported an error");

	ExitThread(error);
	return error;
}