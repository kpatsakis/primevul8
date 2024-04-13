static DWORD WINAPI irp_thread_func(LPVOID arg)
{
	IRP_THREAD_DATA* data = (IRP_THREAD_DATA*)arg;
	UINT error;

	/* blocks until the end of the request */
	if ((error = serial_process_irp(data->serial, data->irp)))
	{
		WLog_ERR(TAG, "serial_process_irp failed with error %" PRIu32 "", error);
		goto error_out;
	}

	EnterCriticalSection(&data->serial->TerminatingIrpThreadsLock);
	data->serial->IrpThreadToBeTerminatedCount++;
	error = data->irp->Complete(data->irp);
	LeaveCriticalSection(&data->serial->TerminatingIrpThreadsLock);
error_out:

	if (error && data->serial->rdpcontext)
		setChannelError(data->serial->rdpcontext, error, "irp_thread_func reported an error");

	/* NB: At this point, the server might already being reusing
	 * the CompletionId whereas the thread is not yet
	 * terminated */
	free(data);
	ExitThread(error);
	return error;
}