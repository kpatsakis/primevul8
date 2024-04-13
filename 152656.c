int sa_read(void *buffer, size_t size)
{
	ssize_t n;

	while (size) {

		if ((n = read(STDIN_FILENO, buffer, size)) < 0) {
			perror("read");
			exit(2);
		}

		if (!n)
			return 1;	/* EOF */

		size -= n;
		buffer = (char *) buffer + n;
	}

	return 0;
}