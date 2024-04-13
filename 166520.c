_PUBLIC_ char **file_lines_load(const char *fname, int *numlines, size_t maxsize, TALLOC_CTX *mem_ctx)
{
	char *p;
	size_t size;

	p = file_load(fname, &size, maxsize, mem_ctx);
	if (!p) return NULL;

	return file_lines_parse(p, size, numlines, mem_ctx);
}