static UINT serial_process_irp_device_control(SERIAL_DEVICE* serial, IRP* irp)
{
	UINT32 IoControlCode;
	UINT32 InputBufferLength;
	BYTE* InputBuffer = NULL;
	UINT32 OutputBufferLength;
	BYTE* OutputBuffer = NULL;
	DWORD BytesReturned = 0;

	if (Stream_GetRemainingLength(irp->input) < 32)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, OutputBufferLength); /* OutputBufferLength (4 bytes) */
	Stream_Read_UINT32(irp->input, InputBufferLength);  /* InputBufferLength (4 bytes) */
	Stream_Read_UINT32(irp->input, IoControlCode);      /* IoControlCode (4 bytes) */
	Stream_Seek(irp->input, 20);                        /* Padding (20 bytes) */

	if (Stream_GetRemainingLength(irp->input) < InputBufferLength)
		return ERROR_INVALID_DATA;

	OutputBuffer = (BYTE*)calloc(OutputBufferLength, sizeof(BYTE));

	if (OutputBuffer == NULL)
	{
		irp->IoStatus = STATUS_NO_MEMORY;
		goto error_handle;
	}

	InputBuffer = (BYTE*)calloc(InputBufferLength, sizeof(BYTE));

	if (InputBuffer == NULL)
	{
		irp->IoStatus = STATUS_NO_MEMORY;
		goto error_handle;
	}

	Stream_Read(irp->input, InputBuffer, InputBufferLength);
	WLog_Print(serial->log, WLOG_DEBUG,
	           "CommDeviceIoControl: CompletionId=%" PRIu32 ", IoControlCode=[0x%" PRIX32 "] %s",
	           irp->CompletionId, IoControlCode, _comm_serial_ioctl_name(IoControlCode));

	/* FIXME: CommDeviceIoControl to be replaced by DeviceIoControl() */
	if (CommDeviceIoControl(serial->hComm, IoControlCode, InputBuffer, InputBufferLength,
	                        OutputBuffer, OutputBufferLength, &BytesReturned, NULL))
	{
		/* WLog_Print(serial->log, WLOG_DEBUG, "CommDeviceIoControl: CompletionId=%"PRIu32",
		 * IoControlCode=[0x%"PRIX32"] %s done", irp->CompletionId, IoControlCode,
		 * _comm_serial_ioctl_name(IoControlCode)); */
		irp->IoStatus = STATUS_SUCCESS;
	}
	else
	{
		WLog_Print(serial->log, WLOG_DEBUG,
		           "CommDeviceIoControl failure: IoControlCode=[0x%" PRIX32
		           "] %s, last-error: 0x%08" PRIX32 "",
		           IoControlCode, _comm_serial_ioctl_name(IoControlCode), GetLastError());
		irp->IoStatus = _GetLastErrorToIoStatus(serial);
	}

error_handle:
	/* FIXME: find out whether it's required or not to get
	 * BytesReturned == OutputBufferLength when
	 * CommDeviceIoControl returns FALSE */
	assert(OutputBufferLength == BytesReturned);
	Stream_Write_UINT32(irp->output, BytesReturned); /* OutputBufferLength (4 bytes) */

	if (BytesReturned > 0)
	{
		if (!Stream_EnsureRemainingCapacity(irp->output, BytesReturned))
		{
			WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
			free(InputBuffer);
			free(OutputBuffer);
			return CHANNEL_RC_NO_MEMORY;
		}

		Stream_Write(irp->output, OutputBuffer, BytesReturned); /* OutputBuffer */
	}

	/* FIXME: Why at least Windows 2008R2 gets lost with this
	 * extra byte and likely on a IOCTL_SERIAL_SET_BAUD_RATE? The
	 * extra byte is well required according MS-RDPEFS
	 * 2.2.1.5.5 */
	/* else */
	/* { */
	/* 	Stream_Write_UINT8(irp->output, 0); /\* Padding (1 byte) *\/ */
	/* } */
	free(InputBuffer);
	free(OutputBuffer);
	return CHANNEL_RC_OK;
}