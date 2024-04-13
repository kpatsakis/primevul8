ftp_mkdir(ftpbuf_t *ftp, const char *dir)
{
	char *mkd, *end;

	if (ftp == NULL) {
		return NULL;
	}
	if (!ftp_putcmd(ftp, "MKD", dir)) {
		return NULL;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 257) {
		return NULL;
	}
	/* copy out the dir from response */
	if ((mkd = strchr(ftp->inbuf, '"')) == NULL) {
		mkd = estrdup(dir);
		return mkd;
	}
	if ((end = strrchr(++mkd, '"')) == NULL) {
		return NULL;
	}
	*end = 0;
	mkd = estrdup(mkd);
	*end = '"';

	return mkd;
}