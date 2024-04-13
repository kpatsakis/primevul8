static bool valid_floppy_drive_params(const short autodetect[8],
		int native_format)
{
	size_t floppy_type_size = ARRAY_SIZE(floppy_type);
	size_t i = 0;

	for (i = 0; i < 8; ++i) {
		if (autodetect[i] < 0 ||
		    autodetect[i] >= floppy_type_size)
			return false;
	}

	if (native_format < 0 || native_format >= floppy_type_size)
		return false;

	return true;
}