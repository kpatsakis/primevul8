wb_set_offset(struct archive_write *a, int64_t off)
{
	struct iso9660 *iso9660 = (struct iso9660 *)a->format_data;
	int64_t used, ext_bytes;

	if (iso9660->wbuff_type != WB_TO_TEMP) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Internal Programing error: iso9660:wb_set_offset()");
		return (ARCHIVE_FATAL);
	}

	used = sizeof(iso9660->wbuff) - iso9660->wbuff_remaining;
	if (iso9660->wbuff_offset + used > iso9660->wbuff_tail)
		iso9660->wbuff_tail = iso9660->wbuff_offset + used;
	if (iso9660->wbuff_offset < iso9660->wbuff_written) {
		if (used > 0 &&
		    write_to_temp(a, iso9660->wbuff, (size_t)used) != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
		iso9660->wbuff_offset = iso9660->wbuff_written;
		lseek(iso9660->temp_fd, iso9660->wbuff_offset, SEEK_SET);
		iso9660->wbuff_remaining = sizeof(iso9660->wbuff);
		used = 0;
	}
	if (off < iso9660->wbuff_offset) {
		/*
		 * Write out waiting data.
		 */
		if (used > 0) {
			if (wb_write_out(a) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		}
		lseek(iso9660->temp_fd, off, SEEK_SET);
		iso9660->wbuff_offset = off;
		iso9660->wbuff_remaining = sizeof(iso9660->wbuff);
	} else if (off <= iso9660->wbuff_tail) {
		iso9660->wbuff_remaining = (size_t)
		    (sizeof(iso9660->wbuff) - (off - iso9660->wbuff_offset));
	} else {
		ext_bytes = off - iso9660->wbuff_tail;
		iso9660->wbuff_remaining = (size_t)(sizeof(iso9660->wbuff)
		   - (iso9660->wbuff_tail - iso9660->wbuff_offset));
		while (ext_bytes >= (int64_t)iso9660->wbuff_remaining) {
			if (write_null(a, (size_t)iso9660->wbuff_remaining)
			    != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
			ext_bytes -= iso9660->wbuff_remaining;
		}
		if (ext_bytes > 0) {
			if (write_null(a, (size_t)ext_bytes) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		}
	}
	return (ARCHIVE_OK);
}