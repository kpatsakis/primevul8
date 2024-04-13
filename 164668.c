ftp_gc(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return;
	}
	if (ftp->pwd) {
		efree(ftp->pwd);
		ftp->pwd = NULL;
	}
	if (ftp->syst) {
		efree(ftp->syst);
		ftp->syst = NULL;
	}
}