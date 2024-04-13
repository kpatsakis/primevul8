ftp_rename(ftpbuf_t *ftp, const char *src, const char *dest)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "RNFR", src)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 350) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "RNTO", dest)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}