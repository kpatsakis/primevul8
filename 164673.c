ftp_list(ftpbuf_t *ftp, const char *path, int recursive TSRMLS_DC)
{
	return ftp_genlist(ftp, ((recursive) ? "LIST -R" : "LIST"), path TSRMLS_CC);
}