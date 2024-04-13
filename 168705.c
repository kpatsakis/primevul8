static void create_irp_thread(SERIAL_DEVICE* serial, IRP* irp)
{
	IRP_THREAD_DATA* data = NULL;
	HANDLE irpThread;
	HANDLE previousIrpThread;
	uintptr_t key;
	/* for a test/debug purpose, uncomment the code below to get a
	 * single thread for all IRPs. NB: two IRPs could not be
	 * processed at the same time, typically two concurent
	 * Read/Write operations could block each other. */
	/* serial_process_irp(serial, irp); */
	/* irp->Complete(irp); */
	/* return; */
	/* NOTE: for good or bad, this implementation relies on the
	 * server to avoid a flooding of requests. see also _purge().
	 */
	EnterCriticalSection(&serial->TerminatingIrpThreadsLock);

	while (serial->IrpThreadToBeTerminatedCount > 0)
	{
		/* Cleaning up termitating and pending irp
		 * threads. See also: irp_thread_func() */
		HANDLE irpThread;
		ULONG_PTR* ids;
		int i, nbIds;
		nbIds = ListDictionary_GetKeys(serial->IrpThreads, &ids);

		for (i = 0; i < nbIds; i++)
		{
			/* Checking if ids[i] is terminating or pending */
			DWORD waitResult;
			ULONG_PTR id = ids[i];
			irpThread = ListDictionary_GetItemValue(serial->IrpThreads, (void*)id);
			/* FIXME: not quite sure a zero timeout is a good thing to check whether a thread is
			 * stil alived or not */
			waitResult = WaitForSingleObject(irpThread, 0);

			if (waitResult == WAIT_OBJECT_0)
			{
				/* terminating thread */
				/* WLog_Print(serial->log, WLOG_DEBUG, "IRP thread with CompletionId=%"PRIuz"
				 * naturally died", id); */
				CloseHandle(irpThread);
				ListDictionary_Remove(serial->IrpThreads, (void*)id);
				serial->IrpThreadToBeTerminatedCount--;
			}
			else if (waitResult != WAIT_TIMEOUT)
			{
				/* unexpected thread state */
				WLog_Print(serial->log, WLOG_WARN,
				           "WaitForSingleObject, got an unexpected result=0x%" PRIX32 "\n",
				           waitResult);
				assert(FALSE);
			}

			/* pending thread (but not yet terminating thread) if waitResult == WAIT_TIMEOUT */
		}

		if (serial->IrpThreadToBeTerminatedCount > 0)
		{
			WLog_Print(serial->log, WLOG_DEBUG, "%" PRIu32 " IRP thread(s) not yet terminated",
			           serial->IrpThreadToBeTerminatedCount);
			Sleep(1); /* 1 ms */
		}

		free(ids);
	}

	LeaveCriticalSection(&serial->TerminatingIrpThreadsLock);
	/* NB: At this point and thanks to the synchronization we're
	 * sure that the incoming IRP uses well a recycled
	 * CompletionId or the server sent again an IRP already posted
	 * which didn't get yet a response (this later server behavior
	 * at least observed with IOCTL_SERIAL_WAIT_ON_MASK and
	 * mstsc.exe).
	 *
	 * FIXME: behavior documented somewhere? behavior not yet
	 * observed with FreeRDP).
	 */
	key = irp->CompletionId;
	previousIrpThread = ListDictionary_GetItemValue(serial->IrpThreads, (void*)key);

	if (previousIrpThread)
	{
		/* Thread still alived <=> Request still pending */
		WLog_Print(serial->log, WLOG_DEBUG,
		           "IRP recall: IRP with the CompletionId=%" PRIu32 " not yet completed!",
		           irp->CompletionId);
		assert(FALSE); /* unimplemented */
		/* TODO: asserts that previousIrpThread handles well
		 * the same request by checking more details. Need an
		 * access to the IRP object used by previousIrpThread
		 */
		/* TODO: taking over the pending IRP or sending a kind
		 * of wake up signal to accelerate the pending
		 * request
		 *
		 * To be considered:
		 *   if (IoControlCode == IOCTL_SERIAL_WAIT_ON_MASK) {
		 *       pComm->PendingEvents |= SERIAL_EV_FREERDP_*;
		 *   }
		 */
		irp->Discard(irp);
		return;
	}

	if (ListDictionary_Count(serial->IrpThreads) >= MAX_IRP_THREADS)
	{
		WLog_Print(serial->log, WLOG_WARN,
		           "Number of IRP threads threshold reached: %d, keep on anyway",
		           ListDictionary_Count(serial->IrpThreads));
		assert(FALSE); /* unimplemented */
		               /* TODO: MAX_IRP_THREADS has been thought to avoid a
		                * flooding of pending requests. Use
		                * WaitForMultipleObjects() when available in winpr
		                * for threads.
		                */
	}

	/* error_handle to be used ... */
	data = (IRP_THREAD_DATA*)calloc(1, sizeof(IRP_THREAD_DATA));

	if (data == NULL)
	{
		WLog_Print(serial->log, WLOG_WARN, "Could not allocate a new IRP_THREAD_DATA.");
		goto error_handle;
	}

	data->serial = serial;
	data->irp = irp;
	/* data freed by irp_thread_func */
	irpThread = CreateThread(NULL, 0, irp_thread_func, (void*)data, 0, NULL);

	if (irpThread == INVALID_HANDLE_VALUE)
	{
		WLog_Print(serial->log, WLOG_WARN, "Could not allocate a new IRP thread.");
		goto error_handle;
	}

	key = irp->CompletionId;

	if (!ListDictionary_Add(serial->IrpThreads, (void*)key, irpThread))
	{
		WLog_ERR(TAG, "ListDictionary_Add failed!");
		goto error_handle;
	}

	return;
error_handle:
	irp->IoStatus = STATUS_NO_MEMORY;
	irp->Complete(irp);
	free(data);
}