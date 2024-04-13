iso9660_write_data(struct archive_write *a, const void *buff, size_t s)
{
	struct iso9660 *iso9660 = a->format_data;
	ssize_t r;

	if (iso9660->cur_file == NULL)
		return (0);
	if (archive_entry_filetype(iso9660->cur_file->entry) != AE_IFREG)
		return (0);
	if (s > iso9660->bytes_remaining)
		s = (size_t)iso9660->bytes_remaining;
	if (s == 0)
		return (0);

	r = write_iso9660_data(a, buff, s);
	if (r > 0)
		iso9660->bytes_remaining -= r;
	return (r);
}