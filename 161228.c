write_iso9660_data(struct archive_write *a, const void *buff, size_t s)
{
	struct iso9660 *iso9660 = a->format_data;
	size_t ws;

	if (iso9660->temp_fd < 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Couldn't create temporary file");
		return (ARCHIVE_FATAL);
	}

	ws = s;
	if (iso9660->need_multi_extent &&
	    (iso9660->cur_file->cur_content->size + ws) >=
	      (MULTI_EXTENT_SIZE - LOGICAL_BLOCK_SIZE)) {
		struct content *con;
		size_t ts;

		ts = (size_t)(MULTI_EXTENT_SIZE - LOGICAL_BLOCK_SIZE -
		    iso9660->cur_file->cur_content->size);

		if (iso9660->zisofs.detect_magic)
			zisofs_detect_magic(a, buff, ts);

		if (iso9660->zisofs.making) {
			if (zisofs_write_to_temp(a, buff, ts) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		} else {
			if (wb_write_to_temp(a, buff, ts) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
			iso9660->cur_file->cur_content->size += ts;
		}

		/* Write padding. */
		if (wb_write_padding_to_temp(a,
		    iso9660->cur_file->cur_content->size) != ARCHIVE_OK)
			return (ARCHIVE_FATAL);

		/* Compute the logical block number. */
		iso9660->cur_file->cur_content->blocks = (int)
		    ((iso9660->cur_file->cur_content->size
		     + LOGICAL_BLOCK_SIZE -1) >> LOGICAL_BLOCK_BITS);

		/*
		 * Make next extent.
		 */
		ws -= ts;
		buff = (const void *)(((const unsigned char *)buff) + ts);
		/* Make a content for next extent. */
		con = calloc(1, sizeof(*con));
		if (con == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate content data");
			return (ARCHIVE_FATAL);
		}
		con->offset_of_temp = wb_offset(a);
		iso9660->cur_file->cur_content->next = con;
		iso9660->cur_file->cur_content = con;
#ifdef HAVE_ZLIB_H
		iso9660->zisofs.block_offset = 0;
#endif
	}

	if (iso9660->zisofs.detect_magic)
		zisofs_detect_magic(a, buff, ws);

	if (iso9660->zisofs.making) {
		if (zisofs_write_to_temp(a, buff, ws) != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	} else {
		if (wb_write_to_temp(a, buff, ws) != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
		iso9660->cur_file->cur_content->size += ws;
	}

	return (s);
}