static UINT printer_free(DEVICE* device)
{
	IRP* irp;
	PRINTER_DEVICE* printer_dev = (PRINTER_DEVICE*)device;
	UINT error;
	SetEvent(printer_dev->stopEvent);

	if (WaitForSingleObject(printer_dev->thread, INFINITE) == WAIT_FAILED)
	{
		error = GetLastError();
		WLog_ERR(TAG, "WaitForSingleObject failed with error %" PRIu32 "", error);

		/* The analyzer is confused by this premature return value.
		 * Since this case can not be handled gracefully silence the
		 * analyzer here. */
#ifndef __clang_analyzer__
		return error;
#endif
	}

	while ((irp = (IRP*)InterlockedPopEntrySList(printer_dev->pIrpList)) != NULL)
		irp->Discard(irp);

	CloseHandle(printer_dev->thread);
	CloseHandle(printer_dev->stopEvent);
	CloseHandle(printer_dev->event);
	_aligned_free(printer_dev->pIrpList);

	if (printer_dev->printer)
		printer_dev->printer->ReleaseRef(printer_dev->printer);

	Stream_Free(printer_dev->device.data, TRUE);
	free(printer_dev);
	return CHANNEL_RC_OK;
}