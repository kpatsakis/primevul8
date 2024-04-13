exif_data_log (ExifData *data, ExifLog *log)
{
	unsigned int i;

	if (!data || !data->priv) 
		return;
	exif_log_unref (data->priv->log);
	data->priv->log = log;
	exif_log_ref (log);

	for (i = 0; i < EXIF_IFD_COUNT; i++)
		exif_content_log (data->ifd[i], log);
}