interpret_maker_note(ExifData *data, const unsigned char *d, unsigned int ds)
{
	int mnoteid;
	ExifEntry* e = exif_data_get_entry (data, EXIF_TAG_MAKER_NOTE);
	if (!e)
		return;
	
	if ((mnoteid = exif_mnote_data_olympus_identify (data, e)) != 0) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG,
			"ExifData", "Olympus MakerNote variant type %d", mnoteid);
		data->priv->md = exif_mnote_data_olympus_new (data->priv->mem);

	} else if ((mnoteid = exif_mnote_data_canon_identify (data, e)) != 0) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG,
			"ExifData", "Canon MakerNote variant type %d", mnoteid);
		data->priv->md = exif_mnote_data_canon_new (data->priv->mem, data->priv->options);

	} else if ((mnoteid = exif_mnote_data_fuji_identify (data, e)) != 0) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG,
			"ExifData", "Fuji MakerNote variant type %d", mnoteid);
		data->priv->md = exif_mnote_data_fuji_new (data->priv->mem);

	/* NOTE: Must do Pentax detection last because some of the
	 * heuristics are pretty general. */
	} else if ((mnoteid = exif_mnote_data_pentax_identify (data, e)) != 0) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG,
			"ExifData", "Pentax MakerNote variant type %d", mnoteid);
		data->priv->md = exif_mnote_data_pentax_new (data->priv->mem);
	}

	/* 
	 * If we are able to interpret the maker note, do so.
	 */
	if (data->priv->md) {
		exif_mnote_data_log (data->priv->md, data->priv->log);
		exif_mnote_data_set_byte_order (data->priv->md,
						data->priv->order);
		exif_mnote_data_set_offset (data->priv->md,
					    data->priv->offset_mnote);
		exif_mnote_data_load (data->priv->md, d, ds);
	}
}