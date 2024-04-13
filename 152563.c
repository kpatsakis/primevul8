exif_data_save_data (ExifData *data, unsigned char **d, unsigned int *ds)
{
	if (ds)
		*ds = 0;	/* This means something went wrong */

	if (!data || !d || !ds)
		return;

	/* Header */
	*ds = 14;
	*d = exif_data_alloc (data, *ds);
	if (!*d)  {
		*ds = 0;
		return;
	}
	memcpy (*d, ExifHeader, 6);

	/* Order (offset 6) */
	if (data->priv->order == EXIF_BYTE_ORDER_INTEL) {
		memcpy (*d + 6, "II", 2);
	} else {
		memcpy (*d + 6, "MM", 2);
	}

	/* Fixed value (2 bytes, offset 8) */
	exif_set_short (*d + 8, data->priv->order, 0x002a);

	/*
	 * IFD 0 offset (4 bytes, offset 10).
	 * We will start 8 bytes after the
	 * EXIF header (2 bytes for order, another 2 for the test, and
	 * 4 bytes for the IFD 0 offset make 8 bytes together).
	 */
	exif_set_long (*d + 10, data->priv->order, 8);

	/* Now save IFD 0. IFD 1 will be saved automatically. */
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
		  "Saving IFDs...");
	exif_data_save_data_content (data, data->ifd[EXIF_IFD_0], d, ds,
				     *ds - 6);
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
		  "Saved %i byte(s) EXIF data.", *ds);
}