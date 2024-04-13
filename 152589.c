exif_data_new (void)
{
	ExifMem *mem = exif_mem_new_default ();
	ExifData *d = exif_data_new_mem (mem);

	exif_mem_unref (mem);

	return d;
}