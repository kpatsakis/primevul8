static BOOL printer_update_to_config(const rdpSettings* settings, const WCHAR* name, size_t length,
                                     const BYTE* data, size_t datalen)
{
	BOOL rc = FALSE;
	char* path = get_printer_config_path(settings, name, length);
	rc = printer_write_setting(path, PRN_CONF_DATA, data, datalen);
	free(path);
	return rc;
}