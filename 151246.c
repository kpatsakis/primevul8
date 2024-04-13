void handle_invalid_sa_file(int fd, struct file_magic *file_magic, char *file,
			    int n)
{
	fprintf(stderr, _("Invalid system activity file: %s\n"), file);

	if (n == FILE_MAGIC_SIZE) {
		if ((file_magic->sysstat_magic == SYSSTAT_MAGIC) || (file_magic->sysstat_magic == SYSSTAT_MAGIC_SWAPPED)) {
			/* This is a sysstat file, but this file has an old format */
			display_sa_file_version(stderr, file_magic);

			fprintf(stderr,
				_("Current sysstat version cannot read the format of this file (%#x)\n"),
				file_magic->sysstat_magic == SYSSTAT_MAGIC ?
				file_magic->format_magic : __builtin_bswap16(file_magic->format_magic));
		}
	}

	close (fd);
	exit(3);
}