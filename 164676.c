ftp_size(ftpbuf_t *ftp, const char *path)
{
	if (ftp == NULL) {
		return -1;
	}
	if (!ftp_type(ftp, FTPTYPE_IMAGE)) {
		return -1;
	}
	if (!ftp_putcmd(ftp, "SIZE", path)) {
		return -1;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 213) {
		return -1;
	}
	return atol(ftp->inbuf);
}