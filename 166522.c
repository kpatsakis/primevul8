_PUBLIC_ char *fd_load(int fd, size_t *psize, size_t maxsize, TALLOC_CTX *mem_ctx)
{
	struct stat sbuf;
	char *p;
	size_t size;

	if (fstat(fd, &sbuf) != 0) return NULL;

	size = sbuf.st_size;

	if (maxsize) {
		size = MIN(size, maxsize);
	}

	p = (char *)talloc_size(mem_ctx, size+1);
	if (!p) return NULL;

	if (read(fd, p, size) != size) {
		talloc_free(p);
		return NULL;
	}
	p[size] = 0;

	if (psize) *psize = size;

	return p;
}