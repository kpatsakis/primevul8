iso9660_finish_entry(struct archive_write *a)
{
	struct iso9660 *iso9660 = a->format_data;

	if (iso9660->cur_file == NULL)
		return (ARCHIVE_OK);
	if (archive_entry_filetype(iso9660->cur_file->entry) != AE_IFREG)
		return (ARCHIVE_OK);
	if (iso9660->cur_file->content.size == 0)
		return (ARCHIVE_OK);

	/* If there are unwritten data, write null data instead. */
	while (iso9660->bytes_remaining > 0) {
		size_t s;

		s = (iso9660->bytes_remaining > a->null_length)?
		    a->null_length: (size_t)iso9660->bytes_remaining;
		if (write_iso9660_data(a, a->nulls, s) < 0)
			return (ARCHIVE_FATAL);
		iso9660->bytes_remaining -= s;
	}

	if (iso9660->zisofs.making && zisofs_finish_entry(a) != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/* Write padding. */
	if (wb_write_padding_to_temp(a, iso9660->cur_file->cur_content->size)
	    != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/* Compute the logical block number. */
	iso9660->cur_file->cur_content->blocks = (int)
	    ((iso9660->cur_file->cur_content->size
	     + LOGICAL_BLOCK_SIZE -1) >> LOGICAL_BLOCK_BITS);

	/* Add the current file to data file list. */
	isofile_add_data_file(iso9660, iso9660->cur_file);

	return (ARCHIVE_OK);
}