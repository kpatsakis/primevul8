exif_data_ref (ExifData *data)
{
	if (!data)
		return;

	data->priv->ref_count++;
}