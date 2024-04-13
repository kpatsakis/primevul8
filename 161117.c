wb_write_to_temp(struct archive_write *a, const void *buff, size_t s)
{
	const char *xp = buff;
	size_t xs = s;

	/*
	 * If a written data size is big enough to use system-call
	 * and there is no waiting data, this calls write_to_temp() in
	 * order to reduce a extra memory copy.
	 */
	if (wb_remaining(a) == wb_buffmax() && s > (1024 * 16)) {
		struct iso9660 *iso9660 = (struct iso9660 *)a->format_data;
		xs = s % LOGICAL_BLOCK_SIZE;
		iso9660->wbuff_offset += s - xs;
		if (write_to_temp(a, buff, s - xs) != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
		if (xs == 0)
			return (ARCHIVE_OK);
		xp += s - xs;
	}

	while (xs) {
		size_t size = xs;
		if (size > wb_remaining(a))
			size = wb_remaining(a);
		memcpy(wb_buffptr(a), xp, size);
		if (wb_consume(a, size) != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
		xs -= size;
		xp += size;
	}
	return (ARCHIVE_OK);
}