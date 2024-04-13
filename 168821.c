static BOOL printer_save_default_config(const rdpSettings* settings, rdpPrinter* printer)
{
	BOOL res = FALSE;
	WCHAR* wname = NULL;
	WCHAR* driver = NULL;
	size_t wlen, dlen;
	char* path = NULL;
	int rc;

	if (!settings || !printer)
		return FALSE;

	rc = ConvertToUnicode(CP_UTF8, 0, printer->name, -1, &wname, 0);

	if (rc <= 0)
		goto fail;

	rc = ConvertToUnicode(CP_UTF8, 0, printer->driver, -1, &driver, 0);

	if (rc <= 0)
		goto fail;

	wlen = _wcslen(wname) + 1;
	dlen = _wcslen(driver) + 1;
	path = get_printer_config_path(settings, wname, wlen * sizeof(WCHAR));

	if (!path)
		goto fail;

	if (dlen > 1)
	{
		if (!printer_write_setting(path, PRN_CONF_DRIVER, driver, dlen * sizeof(WCHAR)))
			goto fail;
	}

	res = TRUE;
fail:
	free(path);
	free(wname);
	free(driver);
	return res;
}