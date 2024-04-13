ftp_getresp(ftpbuf_t *ftp)
{
	char *buf;

	if (ftp == NULL) {
		return 0;
	}
	buf = ftp->inbuf;
	ftp->resp = 0;

	while (1) {

		if (!ftp_readline(ftp)) {
			return 0;
		}

		/* Break out when the end-tag is found */
		if (isdigit(ftp->inbuf[0]) && isdigit(ftp->inbuf[1]) && isdigit(ftp->inbuf[2]) && ftp->inbuf[3] == ' ') {
			break;
		}
	}

	/* translate the tag */
	if (!isdigit(ftp->inbuf[0]) || !isdigit(ftp->inbuf[1]) || !isdigit(ftp->inbuf[2])) {
		return 0;
	}

	ftp->resp = 100 * (ftp->inbuf[0] - '0') + 10 * (ftp->inbuf[1] - '0') + (ftp->inbuf[2] - '0');

	memmove(ftp->inbuf, ftp->inbuf + 4, FTP_BUFSIZE - 4);

	if (ftp->extra) {
		ftp->extra -= 4;
	}
	return 1;
}