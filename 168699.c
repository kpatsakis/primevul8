static BOOL printer_move_config(const rdpSettings* settings, const WCHAR* oldName, size_t oldLength,
                                const WCHAR* newName, size_t newLength)
{
	BOOL rc = FALSE;
	char* oldPath = get_printer_config_path(settings, oldName, oldLength);
	char* newPath = get_printer_config_path(settings, newName, newLength);

	if (printer_config_valid(oldPath))
		rc = MoveFileA(oldPath, newPath);

	free(oldPath);
	free(newPath);
	return rc;
}