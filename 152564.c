exif_data_new_from_file (const char *path)
{
	ExifData *edata;
	ExifLoader *loader;

	loader = exif_loader_new ();
	exif_loader_write_file (loader, path);
	edata = exif_loader_get_data (loader);
	exif_loader_unref (loader);

	return (edata);
}