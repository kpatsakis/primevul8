static BOOL printer_write_setting(const char* path, prn_conf_t type, const void* data,
                                  size_t length)
{
	DWORD written = 0;
	BOOL rc = FALSE;
	HANDLE file;
	size_t b64len;
	char* base64 = NULL;
	const char* name = filemap[type];
	char* abs = GetCombinedPath(path, name);

	if (!abs)
		return FALSE;

	file = CreateFileA(abs, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	free(abs);

	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	if (length > 0)
	{
		base64 = crypto_base64_encode(data, length);

		if (!base64)
			goto fail;

		/* base64 char represents 6bit -> 4*(n/3) is the length which is
		 * always smaller than 2*n */
		b64len = strnlen(base64, 2 * length);
		rc = WriteFile(file, base64, b64len, &written, NULL);

		if (b64len != written)
			rc = FALSE;
	}
	else
		rc = TRUE;

fail:
	CloseHandle(file);
	free(base64);
	return rc;
}