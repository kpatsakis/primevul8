exif_data_get_byte_order (ExifData *data)
{
	if (!data)
		return (0);

	return (data->priv->order);
}