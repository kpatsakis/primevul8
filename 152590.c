exif_data_set_byte_order (ExifData *data, ExifByteOrder order)
{
	ByteOrderChangeData d;

	if (!data || (order == data->priv->order))
		return;

	d.old = data->priv->order;
	d.new = order;
	exif_data_foreach_content (data, content_set_byte_order, &d);
	data->priv->order = order;
	if (data->priv->md)
		exif_mnote_data_set_byte_order (data->priv->md, order);
}