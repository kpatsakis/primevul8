exif_data_set_option (ExifData *d, ExifDataOption o)
{
	if (!d) 
		return;

	d->priv->options |= o;
}