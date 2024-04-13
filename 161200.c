write_to_temp(struct archive_write *a, const void *buff, size_t s)
{
	struct iso9660 *iso9660 = a->format_data;
	ssize_t written;
	const unsigned char *b;

	b = (const unsigned char *)buff;
	while (s) {
		written = write(iso9660->temp_fd, b, s);
		if (written < 0) {
			archive_set_error(&a->archive, errno,
			    "Can't write to temporary file");
			return (ARCHIVE_FATAL);
		}
		s -= written;
		b += written;
	}
	return (ARCHIVE_OK);
}