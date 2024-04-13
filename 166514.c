_PUBLIC_ char **fd_lines_load(int fd, int *numlines, size_t maxsize, TALLOC_CTX *mem_ctx)
{
	char *p;
	size_t size;

	p = fd_load(fd, &size, maxsize, mem_ctx);
	if (!p) return NULL;

	return file_lines_parse(p, size, numlines, mem_ctx);
}