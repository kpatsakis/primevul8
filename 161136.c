write_file_contents(struct archive_write *a, int64_t offset, int64_t size)
{
	struct iso9660 *iso9660 = a->format_data;
	int r;

	lseek(iso9660->temp_fd, offset, SEEK_SET);

	while (size) {
		size_t rsize;
		ssize_t rs;
		unsigned char *wb;

		wb = wb_buffptr(a);
		rsize = wb_remaining(a);
		if (rsize > (size_t)size)
			rsize = (size_t)size;
		rs = read(iso9660->temp_fd, wb, rsize);
		if (rs <= 0) {
			archive_set_error(&a->archive, errno,
			    "Can't read temporary file(%jd)", (intmax_t)rs);
			return (ARCHIVE_FATAL);
		}
		size -= rs;
		r = wb_consume(a, rs);
		if (r < 0)
			return (r);
	}
	return (ARCHIVE_OK);
}