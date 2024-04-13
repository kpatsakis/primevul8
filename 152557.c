exif_data_new_from_data (const unsigned char *data, unsigned int size)
{
	ExifData *edata;

	edata = exif_data_new ();
	exif_data_load_data (edata, data, size);
	return (edata);
}