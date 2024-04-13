static UINT serial_process_irp_close(SERIAL_DEVICE* serial, IRP* irp)
{
	if (Stream_GetRemainingLength(irp->input) < 32)
		return ERROR_INVALID_DATA;

	Stream_Seek(irp->input, 32); /* Padding (32 bytes) */

	if (!CloseHandle(serial->hComm))
	{
		WLog_Print(serial->log, WLOG_WARN, "CloseHandle failure: %s (%" PRIu32 ") closed.",
		           serial->device.name, irp->device->id);
		irp->IoStatus = STATUS_UNSUCCESSFUL;
		goto error_handle;
	}

	WLog_Print(serial->log, WLOG_DEBUG, "%s (DeviceId: %" PRIu32 ", FileId: %" PRIu32 ") closed.",
	           serial->device.name, irp->device->id, irp->FileId);
	serial->hComm = NULL;
	irp->IoStatus = STATUS_SUCCESS;
error_handle:
	Stream_Zero(irp->output, 5); /* Padding (5 bytes) */
	return CHANNEL_RC_OK;
}