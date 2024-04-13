ftp_exec(ftpbuf_t *ftp, const char *cmd)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "SITE EXEC", cmd)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 200) {
		return 0;
	}

	return 1;
}