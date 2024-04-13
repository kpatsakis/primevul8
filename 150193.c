
private void
mdebug(uint32_t offset, const char *str, size_t len)
{
	(void) fprintf(stderr, "mget/%zu @%d: ", len, offset);
	file_showstr(stderr, str, len);
	(void) fputc('\n', stderr);