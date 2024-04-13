exif_data_dump (ExifData *data)
{
	unsigned int i;

	if (!data)
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		if (data->ifd[i] && data->ifd[i]->count) {
			printf ("Dumping IFD '%s'...\n",
				exif_ifd_get_name (i));
			exif_content_dump (data->ifd[i], 0);
		}
	}

	if (data->data) {
		printf ("%i byte(s) thumbnail data available: ", data->size);
		if (data->size >= 4) {
			printf ("0x%02x 0x%02x ... 0x%02x 0x%02x\n",
				data->data[0], data->data[1],
				data->data[data->size - 2],
				data->data[data->size - 1]);
		}
	}
}