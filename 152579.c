exif_data_set_data_type (ExifData *d, ExifDataType dt)
{
	if (!d || !d->priv) 
		return;

	d->priv->data_type = dt;
}