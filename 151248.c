void guess_sa_name(char *sa_dir, struct tm *rectime, int *sa_name)
{
	char filename[MAX_FILE_LEN];
	struct stat sb;
	time_t sa_mtime;
	long nsec;

	/* Use saDD by default */
	*sa_name = 0;

	/* Look for saYYYYMMDD */
	snprintf(filename, MAX_FILE_LEN,
		 "%s/sa%04d%02d%02d", sa_dir,
		 rectime->tm_year + 1900,
		 rectime->tm_mon + 1,
		 rectime->tm_mday);
	filename[MAX_FILE_LEN - 1] = '\0';

	if (stat(filename, &sb) < 0)
		/* Cannot find or access saYYYYMMDD, so use saDD */
		return;
	sa_mtime = sb.st_mtime;
	nsec = sb.st_mtim.tv_nsec;

	/* Look for saDD */
	snprintf(filename, MAX_FILE_LEN,
		 "%s/sa%02d", sa_dir,
		 rectime->tm_mday);
	filename[MAX_FILE_LEN - 1] = '\0';

	if (stat(filename, &sb) < 0) {
		/* Cannot find or access saDD, so use saYYYYMMDD */
		*sa_name = 1;
		return;
	}

	if ((sa_mtime > sb.st_mtime) ||
	    ((sa_mtime == sb.st_mtime) && (nsec > sb.st_mtim.tv_nsec))) {
		/* saYYYYMMDD is more recent than saDD, so use it */
		*sa_name = 1;
	}
}