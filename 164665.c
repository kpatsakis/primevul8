ftp_quit(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return 0;
	}

	if (!ftp_putcmd(ftp, "QUIT", NULL)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 221) {
		return 0;
	}

	if (ftp->pwd) {
		efree(ftp->pwd);
		ftp->pwd = NULL;
	}

	return 1;
}