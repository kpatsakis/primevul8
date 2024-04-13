bool file_compare(const char *path1, const char *path2)
{
	size_t size1, size2;
	char *p1, *p2;
	TALLOC_CTX *mem_ctx = talloc_new(NULL);

	p1 = file_load(path1, &size1, 0, mem_ctx);
	p2 = file_load(path2, &size2, 0, mem_ctx);
	if (!p1 || !p2 || size1 != size2) {
		talloc_free(mem_ctx);
		return false;
	}
	if (memcmp(p1, p2, size1) != 0) {
		talloc_free(mem_ctx);
		return false;
	}
	talloc_free(mem_ctx);
	return true;
}