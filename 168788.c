static UINT printer_custom_component(DEVICE* device, UINT16 component, UINT16 packetId, wStream* s)
{
	UINT32 eventID;
	PRINTER_DEVICE* printer_dev = (PRINTER_DEVICE*)device;
	const rdpSettings* settings = printer_dev->rdpcontext->settings;

	if (component != RDPDR_CTYP_PRN)
		return ERROR_INVALID_DATA;

	if (Stream_GetRemainingLength(s) < 4)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(s, eventID);

	switch (packetId)
	{
		case PAKID_PRN_CACHE_DATA:
			switch (eventID)
			{
				case RDPDR_ADD_PRINTER_EVENT:
				{
					char PortDosName[8];
					UINT32 PnPNameLen, DriverNameLen, PrintNameLen, CacheFieldsLen;
					const WCHAR *PnPName, *DriverName, *PrinterName;
					const BYTE* CachedPrinterConfigData;

					if (Stream_GetRemainingLength(s) < 24)
						return ERROR_INVALID_DATA;

					Stream_Read(s, PortDosName, sizeof(PortDosName));
					Stream_Read_UINT32(s, PnPNameLen);
					Stream_Read_UINT32(s, DriverNameLen);
					Stream_Read_UINT32(s, PrintNameLen);
					Stream_Read_UINT32(s, CacheFieldsLen);

					if (Stream_GetRemainingLength(s) < PnPNameLen)
						return ERROR_INVALID_DATA;

					PnPName = (const WCHAR*)Stream_Pointer(s);
					Stream_Seek(s, PnPNameLen);

					if (Stream_GetRemainingLength(s) < DriverNameLen)
						return ERROR_INVALID_DATA;

					DriverName = (const WCHAR*)Stream_Pointer(s);
					Stream_Seek(s, DriverNameLen);

					if (Stream_GetRemainingLength(s) < PrintNameLen)
						return ERROR_INVALID_DATA;

					PrinterName = (const WCHAR*)Stream_Pointer(s);
					Stream_Seek(s, PrintNameLen);

					if (Stream_GetRemainingLength(s) < CacheFieldsLen)
						return ERROR_INVALID_DATA;

					CachedPrinterConfigData = Stream_Pointer(s);
					Stream_Seek(s, CacheFieldsLen);

					if (!printer_save_to_config(settings, PortDosName, sizeof(PortDosName), PnPName,
					                            PnPNameLen, DriverName, DriverNameLen, PrinterName,
					                            PrintNameLen, CachedPrinterConfigData,
					                            CacheFieldsLen))
						return ERROR_INTERNAL_ERROR;
				}
				break;

				case RDPDR_UPDATE_PRINTER_EVENT:
				{
					UINT32 PrinterNameLen, ConfigDataLen;
					const WCHAR* PrinterName;
					const BYTE* ConfigData;

					if (Stream_GetRemainingLength(s) < 8)
						return ERROR_INVALID_DATA;

					Stream_Read_UINT32(s, PrinterNameLen);
					Stream_Read_UINT32(s, ConfigDataLen);

					if (Stream_GetRemainingLength(s) < PrinterNameLen)
						return ERROR_INVALID_DATA;

					PrinterName = (const WCHAR*)Stream_Pointer(s);
					Stream_Seek(s, PrinterNameLen);

					if (Stream_GetRemainingLength(s) < ConfigDataLen)
						return ERROR_INVALID_DATA;

					ConfigData = Stream_Pointer(s);
					Stream_Seek(s, ConfigDataLen);

					if (!printer_update_to_config(settings, PrinterName, PrinterNameLen, ConfigData,
					                              ConfigDataLen))
						return ERROR_INTERNAL_ERROR;
				}
				break;

				case RDPDR_DELETE_PRINTER_EVENT:
				{
					UINT32 PrinterNameLen;
					const WCHAR* PrinterName;

					if (Stream_GetRemainingLength(s) < 4)
						return ERROR_INVALID_DATA;

					Stream_Read_UINT32(s, PrinterNameLen);

					if (Stream_GetRemainingLength(s) < PrinterNameLen)
						return ERROR_INVALID_DATA;

					PrinterName = (const WCHAR*)Stream_Pointer(s);
					Stream_Seek(s, PrinterNameLen);
					printer_remove_config(settings, PrinterName, PrinterNameLen);
				}
				break;

				case RDPDR_RENAME_PRINTER_EVENT:
				{
					UINT32 OldPrinterNameLen, NewPrinterNameLen;
					const WCHAR* OldPrinterName;
					const WCHAR* NewPrinterName;

					if (Stream_GetRemainingLength(s) < 8)
						return ERROR_INVALID_DATA;

					Stream_Read_UINT32(s, OldPrinterNameLen);
					Stream_Read_UINT32(s, NewPrinterNameLen);

					if (Stream_GetRemainingLength(s) < OldPrinterNameLen)
						return ERROR_INVALID_DATA;

					OldPrinterName = (const WCHAR*)Stream_Pointer(s);
					Stream_Seek(s, OldPrinterNameLen);

					if (Stream_GetRemainingLength(s) < NewPrinterNameLen)
						return ERROR_INVALID_DATA;

					NewPrinterName = (const WCHAR*)Stream_Pointer(s);
					Stream_Seek(s, NewPrinterNameLen);

					if (!printer_move_config(settings, OldPrinterName, OldPrinterNameLen,
					                         NewPrinterName, NewPrinterNameLen))
						return ERROR_INTERNAL_ERROR;
				}
				break;

				default:
					WLog_ERR(TAG, "Unknown cache data eventID: 0x%08" PRIX32 "", eventID);
					return ERROR_INVALID_DATA;
			}

			break;

		case PAKID_PRN_USING_XPS:
		{
			UINT32 flags;

			if (Stream_GetRemainingLength(s) < 4)
				return ERROR_INVALID_DATA;

			Stream_Read_UINT32(s, flags);
			WLog_ERR(TAG,
			         "Ignoring unhandled message PAKID_PRN_USING_XPS [printerID=%08" PRIx32
			         ", flags=%08" PRIx32 "]",
			         eventID, flags);
		}
		break;

		default:
			WLog_ERR(TAG, "Unknown printing component packetID: 0x%04" PRIX16 "", packetId);
			return ERROR_INVALID_DATA;
	}

	return CHANNEL_RC_OK;
}