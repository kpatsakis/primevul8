static BOOL printer_save_to_config(const rdpSettings* settings, const char* PortDosName,
                                   size_t PortDosNameLen, const WCHAR* PnPName, size_t PnPNameLen,
                                   const WCHAR* DriverName, size_t DriverNameLen,
                                   const WCHAR* PrinterName, size_t PrintNameLen,
                                   const BYTE* CachedPrinterConfigData, size_t CacheFieldsLen)
{
	BOOL rc = FALSE;
	char* path = get_printer_config_path(settings, PrinterName, PrintNameLen);

	if (!path)
		goto fail;

	if (!printer_write_setting(path, PRN_CONF_PORT, PortDosName, PortDosNameLen))
		goto fail;

	if (!printer_write_setting(path, PRN_CONF_PNP, PnPName, PnPNameLen))
		goto fail;

	if (!printer_write_setting(path, PRN_CONF_DRIVER, DriverName, DriverNameLen))
		goto fail;

	if (!printer_write_setting(path, PRN_CONF_DATA, CachedPrinterConfigData, CacheFieldsLen))
		goto fail;

fail:
	free(path);
	return rc;
}