_PUBLIC_ bool large_file_support(const char *path)
{
	int fd;
	ssize_t ret;
	char c;

	fd = open(path, O_RDWR|O_CREAT, 0600);
	unlink(path);
	if (fd == -1) {
		/* have to assume large files are OK */
		return true;
	}
	ret = pread(fd, &c, 1, ((uint64_t)1)<<32);
	close(fd);
	return ret == 0;
}