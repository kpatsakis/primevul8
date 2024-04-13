truncated_error(struct archive_read *a)
{
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "Truncated LHa header");
	return (ARCHIVE_FATAL);
}