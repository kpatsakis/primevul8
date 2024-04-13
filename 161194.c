wb_consume(struct archive_write *a, size_t size)
{
	struct iso9660 *iso9660 = (struct iso9660 *)a->format_data;

	if (size > iso9660->wbuff_remaining ||
	    iso9660->wbuff_remaining == 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Internal Programing error: iso9660:wb_consume()"
		    " size=%jd, wbuff_remaining=%jd",
		    (intmax_t)size, (intmax_t)iso9660->wbuff_remaining);
		return (ARCHIVE_FATAL);
	}
	iso9660->wbuff_remaining -= size;
	if (iso9660->wbuff_remaining < LOGICAL_BLOCK_SIZE)
		return (wb_write_out(a));
	return (ARCHIVE_OK);
}