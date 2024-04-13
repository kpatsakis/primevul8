static DRIVE_FILE* drive_get_file_by_id(DRIVE_DEVICE* drive, UINT32 id)
{
	DRIVE_FILE* file = NULL;
	void* key = (void*)(size_t)id;

	if (!drive)
		return NULL;

	file = (DRIVE_FILE*)ListDictionary_GetItemValue(drive->files, key);
	return file;
}