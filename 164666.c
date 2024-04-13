ftp_delete(ftpbuf_t *ftp, const char *path)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "DELE", path)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}

	return 1;
}