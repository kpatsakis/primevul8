static BOOL printer_load_from_config(const rdpSettings* settings, rdpPrinter* printer,
                                     PRINTER_DEVICE* printer_dev)
{
	BOOL res = FALSE;
	WCHAR* wname = NULL;
	size_t wlen;
	char* path = NULL;
	int rc;
	UINT32 flags = 0;
	void* DriverName = NULL;
	UINT32 DriverNameLen = 0;
	void* PnPName = NULL;
	UINT32 PnPNameLen = 0;
	void* CachedPrinterConfigData = NULL;
	UINT32 CachedFieldsLen = 0;
	UINT32 PrinterNameLen = 0;

	if (!settings || !printer)
		return FALSE;

	rc = ConvertToUnicode(CP_UTF8, 0, printer->name, -1, &wname, 0);

	if (rc <= 0)
		goto fail;

	wlen = _wcslen(wname) + 1;
	path = get_printer_config_path(settings, wname, wlen * sizeof(WCHAR));
	PrinterNameLen = (wlen + 1) * sizeof(WCHAR);

	if (!path)
		goto fail;

	if (printer->is_default)
		flags |= RDPDR_PRINTER_ANNOUNCE_FLAG_DEFAULTPRINTER;

	if (!printer_read_setting(path, PRN_CONF_PNP, &PnPName, &PnPNameLen))
	{
	}

	if (!printer_read_setting(path, PRN_CONF_DRIVER, &DriverName, &DriverNameLen))
	{
		DriverNameLen =
		    ConvertToUnicode(CP_UTF8, 0, printer->driver, -1, (LPWSTR*)&DriverName, 0) * 2 + 1;
	}

	if (!printer_read_setting(path, PRN_CONF_DATA, &CachedPrinterConfigData, &CachedFieldsLen))
	{
	}

	Stream_SetPosition(printer_dev->device.data, 0);

	if (!Stream_EnsureRemainingCapacity(printer_dev->device.data, 24))
		goto fail;

	Stream_Write_UINT32(printer_dev->device.data, flags);
	Stream_Write_UINT32(printer_dev->device.data, 0);          /* CodePage, reserved */
	Stream_Write_UINT32(printer_dev->device.data, PnPNameLen); /* PnPNameLen */
	Stream_Write_UINT32(printer_dev->device.data, DriverNameLen);
	Stream_Write_UINT32(printer_dev->device.data, PrinterNameLen);
	Stream_Write_UINT32(printer_dev->device.data, CachedFieldsLen);

	if (!Stream_EnsureRemainingCapacity(printer_dev->device.data, PnPNameLen))
		goto fail;

	if (PnPNameLen > 0)
		Stream_Write(printer_dev->device.data, PnPName, PnPNameLen);

	if (!Stream_EnsureRemainingCapacity(printer_dev->device.data, DriverNameLen))
		goto fail;

	Stream_Write(printer_dev->device.data, DriverName, DriverNameLen);

	if (!Stream_EnsureRemainingCapacity(printer_dev->device.data, PrinterNameLen))
		goto fail;

	Stream_Write(printer_dev->device.data, wname, PrinterNameLen);

	if (!Stream_EnsureRemainingCapacity(printer_dev->device.data, CachedFieldsLen))
		goto fail;

	Stream_Write(printer_dev->device.data, CachedPrinterConfigData, CachedFieldsLen);
	res = TRUE;
fail:
	free(path);
	free(wname);
	free(PnPName);
	free(DriverName);
	free(CachedPrinterConfigData);
	return res;
}