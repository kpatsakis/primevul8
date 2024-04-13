static char* get_printer_config_path(const rdpSettings* settings, const WCHAR* name, size_t length)
{
	char* dir = GetCombinedPath(settings->ConfigPath, "printers");
	char* bname = crypto_base64_encode((const BYTE*)name, (int)length);
	char* config = GetCombinedPath(dir, bname);

	if (config && !PathFileExistsA(config))
	{
		if (!PathMakePathA(config, NULL))
		{
			free(config);
			config = NULL;
		}
	}

	free(dir);
	free(bname);
	return config;
}