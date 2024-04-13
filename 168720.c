static UINT32 _GetLastErrorToIoStatus(SERIAL_DEVICE* serial)
{
	/* http://msdn.microsoft.com/en-us/library/ff547466%28v=vs.85%29.aspx#generic_status_values_for_serial_device_control_requests
	 */
	switch (GetLastError())
	{
		case ERROR_BAD_DEVICE:
			return STATUS_INVALID_DEVICE_REQUEST;

		case ERROR_CALL_NOT_IMPLEMENTED:
			return STATUS_NOT_IMPLEMENTED;

		case ERROR_CANCELLED:
			return STATUS_CANCELLED;

		case ERROR_INSUFFICIENT_BUFFER:
			return STATUS_BUFFER_TOO_SMALL; /* NB: STATUS_BUFFER_SIZE_TOO_SMALL not defined  */

		case ERROR_INVALID_DEVICE_OBJECT_PARAMETER: /* eg: SerCx2.sys' _purge() */
			return STATUS_INVALID_DEVICE_STATE;

		case ERROR_INVALID_HANDLE:
			return STATUS_INVALID_DEVICE_REQUEST;

		case ERROR_INVALID_PARAMETER:
			return STATUS_INVALID_PARAMETER;

		case ERROR_IO_DEVICE:
			return STATUS_IO_DEVICE_ERROR;

		case ERROR_IO_PENDING:
			return STATUS_PENDING;

		case ERROR_NOT_SUPPORTED:
			return STATUS_NOT_SUPPORTED;

		case ERROR_TIMEOUT:
			return STATUS_TIMEOUT;
			/* no default */
	}

	WLog_Print(serial->log, WLOG_DEBUG, "unexpected last-error: 0x%08" PRIX32 "", GetLastError());
	return STATUS_UNSUCCESSFUL;
}