exif_data_get_data_type (ExifData *d)
{
	return (d && d->priv) ? d->priv->data_type : EXIF_DATA_TYPE_UNKNOWN;
}