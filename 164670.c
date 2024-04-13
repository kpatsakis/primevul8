ftp_site(ftpbuf_t *ftp, const char *cmd)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "SITE", cmd)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp < 200 || ftp->resp >= 300) {
		return 0;
	}

	return 1;
}