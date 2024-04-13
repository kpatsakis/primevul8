wb_write_padding_to_temp(struct archive_write *a, int64_t csize)
{
	size_t ns;
	int ret;

	ns = (size_t)(csize % LOGICAL_BLOCK_SIZE);
	if (ns != 0)
		ret = write_null(a, LOGICAL_BLOCK_SIZE - ns);
	else
		ret = ARCHIVE_OK;
	return (ret);
}