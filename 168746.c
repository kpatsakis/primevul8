static void terminate_pending_irp_threads(SERIAL_DEVICE* serial)
{
	ULONG_PTR* ids;
	int i, nbIds;
	nbIds = ListDictionary_GetKeys(serial->IrpThreads, &ids);
	WLog_Print(serial->log, WLOG_DEBUG, "Terminating %d IRP thread(s)", nbIds);

	for (i = 0; i < nbIds; i++)
	{
		HANDLE irpThread;
		ULONG_PTR id = ids[i];
		irpThread = ListDictionary_GetItemValue(serial->IrpThreads, (void*)id);
		TerminateThread(irpThread, 0);

		if (WaitForSingleObject(irpThread, INFINITE) == WAIT_FAILED)
		{
			WLog_ERR(TAG, "WaitForSingleObject failed!");
			continue;
		}

		CloseHandle(irpThread);
		WLog_Print(serial->log, WLOG_DEBUG, "IRP thread terminated, CompletionId %p", (void*)id);
	}

	ListDictionary_Clear(serial->IrpThreads);
	free(ids);
}