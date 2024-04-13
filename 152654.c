void seek_file_position(int ifd, int action)
{
	static off_t fpos = -1;
	static unsigned int save_cpu_nr = 0;

	if (action == DO_SAVE) {
		/* Save current file position */
		if ((fpos = lseek(ifd, 0, SEEK_CUR)) < 0) {
			perror("lseek");
			exit(2);
		}
		save_cpu_nr = file_hdr.sa_cpu_nr;
	}
	else if (action == DO_RESTORE) {
		/* Rewind file */
		if ((fpos < 0) || (lseek(ifd, fpos, SEEK_SET) < fpos)) {
			perror("lseek");
			exit(2);
		}
		file_hdr.sa_cpu_nr = save_cpu_nr;
	}
}