ftp_rmdir(ftpbuf_t *ftp, const char *dir)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "RMD", dir)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}