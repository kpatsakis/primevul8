exif_data_unset_option (ExifData *d, ExifDataOption o)
{
	if (!d) 
		return;

	d->priv->options &= ~o;
}