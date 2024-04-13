_PUBLIC_ void *map_file(const char *fname, size_t size)
{
	size_t s2 = 0;
	void *p = NULL;
#ifdef HAVE_MMAP
	int fd;
	fd = open(fname, O_RDONLY, 0);
	if (fd == -1) {
		DEBUG(2,("Failed to load %s - %s\n", fname, strerror(errno)));
		return NULL;
	}
	p = mmap(NULL, size, PROT_READ, MAP_SHARED|MAP_FILE, fd, 0);
	close(fd);
	if (p == MAP_FAILED) {
		DEBUG(1,("Failed to mmap %s - %s\n", fname, strerror(errno)));
		return NULL;
	}
#endif
	if (!p) {
		p = file_load(fname, &s2, 0, NULL);
		if (!p) return NULL;
		if (s2 != size) {
			DEBUG(1,("incorrect size for %s - got %d expected %d\n",
				 fname, (int)s2, (int)size));
			talloc_free(p);
			return NULL;
		}
	}

	return p;
}