int write_all(int fd, const void *buf, int nr_bytes)
{
	int block, offset = 0;
	char *buffer = (char *) buf;

	while (nr_bytes > 0) {
		block = write(fd, &buffer[offset], nr_bytes);

		if (block < 0) {
			if (errno == EINTR)
				continue;
			return block;
		}
		if (block == 0)
			return offset;

		offset += block;
		nr_bytes -= block;
	}

	return offset;
}