ftp_nlist(ftpbuf_t *ftp, const char *path TSRMLS_DC)
{
	return ftp_genlist(ftp, "NLST", path TSRMLS_CC);
}