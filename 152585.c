exif_data_free (ExifData *data)
{
	unsigned int i;
	ExifMem *mem = (data && data->priv) ? data->priv->mem : NULL;

	if (!data) 
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		if (data->ifd[i]) {
			exif_content_unref (data->ifd[i]);
			data->ifd[i] = NULL;
		}
	}

	if (data->data) {
		exif_mem_free (mem, data->data);
		data->data = NULL;
	}

	if (data->priv) {
		if (data->priv->log) {
			exif_log_unref (data->priv->log);
			data->priv->log = NULL;
		}
		if (data->priv->md) {
			exif_mnote_data_unref (data->priv->md);
			data->priv->md = NULL;
		}
		exif_mem_free (mem, data->priv);
		exif_mem_free (mem, data);
	}

	exif_mem_unref (mem);
}