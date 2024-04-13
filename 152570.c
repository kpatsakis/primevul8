exif_data_new_mem (ExifMem *mem)
{
	ExifData *data;
	unsigned int i;

	if (!mem) 
		return NULL;

	data = exif_mem_alloc (mem, sizeof (ExifData));
	if (!data) 
		return (NULL);
	data->priv = exif_mem_alloc (mem, sizeof (ExifDataPrivate));
	if (!data->priv) { 
	  	exif_mem_free (mem, data); 
		return (NULL); 
	}
	data->priv->ref_count = 1;

	data->priv->mem = mem;
	exif_mem_ref (mem);

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		data->ifd[i] = exif_content_new_mem (data->priv->mem);
		if (!data->ifd[i]) {
			exif_data_free (data);
			return (NULL);
		}
		data->ifd[i]->parent = data;
	}

	/* Default options */
#ifndef NO_VERBOSE_TAG_STRINGS
	/*
	 * When the tag list is compiled away, setting this option prevents
	 * any tags from being loaded
	 */
	exif_data_set_option (data, EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS);
#endif
	exif_data_set_option (data, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);

	/* Default data type: none */
	exif_data_set_data_type (data, EXIF_DATA_TYPE_COUNT);

	return (data);
}