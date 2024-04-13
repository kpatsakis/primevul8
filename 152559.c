exif_data_fix (ExifData *d)
{
	exif_data_foreach_content (d, fix_func, NULL);
}