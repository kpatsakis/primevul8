exif_data_alloc (ExifData *data, unsigned int i)
{
	void *d;

	if (!data || !i) 
		return NULL;

	d = exif_mem_alloc (data->priv->mem, i);
	if (d) 
		return d;

	EXIF_LOG_NO_MEMORY (data->priv->log, "ExifData", i);
	return NULL;
}