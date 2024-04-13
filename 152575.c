exif_data_unref (ExifData *data)
{
	if (!data) 
		return;

	data->priv->ref_count--;
	if (!data->priv->ref_count) 
		exif_data_free (data);
}