exif_data_foreach_content (ExifData *data, ExifDataForeachContentFunc func,
			   void *user_data)
{
	unsigned int i;

	if (!data || !func)
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++)
		func (data->ifd[i], user_data);
}