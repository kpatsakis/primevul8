zisofs_write_to_temp(struct archive_write *a, const void *buff, size_t s)
{
	(void)buff; /* UNUSED */
	(void)s; /* UNUSED */
	archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC, "Programing error");
	return (ARCHIVE_FATAL);
}