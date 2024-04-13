static UINT serial_process_irp_read(SERIAL_DEVICE* serial, IRP* irp)
{
	UINT32 Length;
	UINT64 Offset;
	BYTE* buffer = NULL;
	DWORD nbRead = 0;

	if (Stream_GetRemainingLength(irp->input) < 32)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, Length); /* Length (4 bytes) */
	Stream_Read_UINT64(irp->input, Offset); /* Offset (8 bytes) */
	Stream_Seek(irp->input, 20);            /* Padding (20 bytes) */
	buffer = (BYTE*)calloc(Length, sizeof(BYTE));

	if (buffer == NULL)
	{
		irp->IoStatus = STATUS_NO_MEMORY;
		goto error_handle;
	}

	/* MS-RDPESP 3.2.5.1.4: If the Offset field is not set to 0, the value MUST be ignored
	 * assert(Offset == 0);
	 */
	WLog_Print(serial->log, WLOG_DEBUG, "reading %" PRIu32 " bytes from %s", Length,
	           serial->device.name);

	/* FIXME: CommReadFile to be replaced by ReadFile */
	if (CommReadFile(serial->hComm, buffer, Length, &nbRead, NULL))
	{
		irp->IoStatus = STATUS_SUCCESS;
	}
	else
	{
		WLog_Print(serial->log, WLOG_DEBUG,
		           "read failure to %s, nbRead=%" PRIu32 ", last-error: 0x%08" PRIX32 "",
		           serial->device.name, nbRead, GetLastError());
		irp->IoStatus = _GetLastErrorToIoStatus(serial);
	}

	WLog_Print(serial->log, WLOG_DEBUG, "%" PRIu32 " bytes read from %s", nbRead,
	           serial->device.name);
error_handle:
	Stream_Write_UINT32(irp->output, nbRead); /* Length (4 bytes) */

	if (nbRead > 0)
	{
		if (!Stream_EnsureRemainingCapacity(irp->output, nbRead))
		{
			WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
			free(buffer);
			return CHANNEL_RC_NO_MEMORY;
		}

		Stream_Write(irp->output, buffer, nbRead); /* ReadData */
	}

	free(buffer);
	return CHANNEL_RC_OK;
}