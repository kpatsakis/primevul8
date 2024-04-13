exif_data_get_log (ExifData *data)
{
	if (!data || !data->priv) 
		return NULL;
	return data->priv->log;
}