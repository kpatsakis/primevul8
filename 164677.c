ftp_chdir(ftpbuf_t *ftp, const char *dir)
{
	if (ftp == NULL) {
		return 0;
	}

	if (ftp->pwd) {
		efree(ftp->pwd);
		ftp->pwd = NULL;
	}

	if (!ftp_putcmd(ftp, "CWD", dir)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}