ftp_mdtm(ftpbuf_t *ftp, const char *path)
{
	time_t		stamp;
	struct tm	*gmt, tmbuf;
	struct tm	tm;
	char		*ptr;
	int		n;

	if (ftp == NULL) {
		return -1;
	}
	if (!ftp_putcmd(ftp, "MDTM", path)) {
		return -1;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 213) {
		return -1;
	}
	/* parse out the timestamp */
	for (ptr = ftp->inbuf; *ptr && !isdigit(*ptr); ptr++);
	n = sscanf(ptr, "%4u%2u%2u%2u%2u%2u", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
	if (n != 6) {
		return -1;
	}
	tm.tm_year -= 1900;
	tm.tm_mon--;
	tm.tm_isdst = -1;

	/* figure out the GMT offset */
	stamp = time(NULL);
	gmt = php_gmtime_r(&stamp, &tmbuf);
	if (!gmt) {
		return -1;
	}
	gmt->tm_isdst = -1;

	/* apply the GMT offset */
	tm.tm_sec += stamp - mktime(gmt);
	tm.tm_isdst = gmt->tm_isdst;

	stamp = mktime(&tm);

	return stamp;
}