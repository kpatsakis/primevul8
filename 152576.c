exif_data_get_mnote_data (ExifData *d)
{
	return (d && d->priv) ? d->priv->md : NULL;
}