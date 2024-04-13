int sa_fread(int ifd, void *buffer, size_t size, int mode, int oneof)
{
	ssize_t n;

	if ((n = read(ifd, buffer, size)) < 0) {
		fprintf(stderr, _("Error while reading system activity file: %s\n"),
			strerror(errno));
		close(ifd);
		exit(2);
	}

	if (!n && (mode == SOFT_SIZE))
		return 1;	/* EOF */

	if (n < size) {
		fprintf(stderr, _("End of system activity file unexpected\n"));
		if (oneof == UEOF_CONT)
			return 2;
		close(ifd);
		exit(2);
	}

	return 0;
}