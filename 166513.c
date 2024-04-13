_PUBLIC_ char *file_load(const char *fname, size_t *size, size_t maxsize, TALLOC_CTX *mem_ctx)
{
	int fd;
	char *p;

	if (!fname || !*fname) return NULL;
	
	fd = open(fname,O_RDONLY);
	if (fd == -1) return NULL;

	p = fd_load(fd, size, maxsize, mem_ctx);

	close(fd);

	return p;
}