static UINT serial_free(DEVICE* device)
{
	UINT error;
	SERIAL_DEVICE* serial = (SERIAL_DEVICE*)device;
	WLog_Print(serial->log, WLOG_DEBUG, "freeing");
	MessageQueue_PostQuit(serial->MainIrpQueue, 0);

	if (WaitForSingleObject(serial->MainThread, INFINITE) == WAIT_FAILED)
	{
		error = GetLastError();
		WLog_ERR(TAG, "WaitForSingleObject failed with error %" PRIu32 "!", error);
		return error;
	}

	CloseHandle(serial->MainThread);

	if (serial->hComm)
		CloseHandle(serial->hComm);

	/* Clean up resources */
	Stream_Free(serial->device.data, TRUE);
	MessageQueue_Free(serial->MainIrpQueue);
	ListDictionary_Free(serial->IrpThreads);
	DeleteCriticalSection(&serial->TerminatingIrpThreadsLock);
	free(serial);
	return CHANNEL_RC_OK;
}