_PUBLIC_ bool file_save_mode(const char *fname, const void *packet,
			     size_t length, mode_t mode)
{
	int fd;
	fd = open(fname, O_WRONLY|O_CREAT|O_TRUNC, mode);
	if (fd == -1) {
		return false;
	}
	if (write(fd, packet, length) != (size_t)length) {
		close(fd);
		return false;
	}
	close(fd);
	return true;
}