static BOOL printer_read_setting(const char* path, prn_conf_t type, void** data, UINT32* length)
{
	DWORD lowSize, highSize;
	DWORD read = 0;
	BOOL rc = FALSE;
	HANDLE file;
	char* fdata = NULL;
	const char* name = filemap[type];
	char* abs = GetCombinedPath(path, name);

	if (!abs)
		return FALSE;

	file = CreateFileA(abs, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	free(abs);

	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	lowSize = GetFileSize(file, &highSize);

	if ((lowSize == INVALID_FILE_SIZE) || (highSize != 0))
		goto fail;

	if (lowSize != 0)
	{
		fdata = malloc(lowSize);

		if (!fdata)
			goto fail;

		rc = ReadFile(file, fdata, lowSize, &read, NULL);

		if (lowSize != read)
			rc = FALSE;
	}

fail:
	CloseHandle(file);

	if (rc && (lowSize <= INT_MAX))
	{
		int blen = 0;
		crypto_base64_decode(fdata, (int)lowSize, (BYTE**)data, &blen);

		if (*data && (blen > 0))
			*length = (UINT32)blen;
		else
		{
			rc = FALSE;
			*length = 0;
		}
	}
	else
	{
		*length = 0;
		*data = NULL;
	}

	free(fdata);
	return rc;
}