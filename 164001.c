__archive_read_seek(struct archive_read *a, int64_t offset, int whence)
{
	return __archive_read_filter_seek(a->filter, offset, whence);
}