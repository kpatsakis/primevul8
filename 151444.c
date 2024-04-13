GetDataBlock(gdIOCtx *fd, unsigned char *buf, int *ZeroDataBlockP)
{
	int rv;
	int i;

	rv = GetDataBlock_(fd,buf, ZeroDataBlockP);
	if (VERBOSE) {
		char *tmp = NULL;
		if (rv > 0) {
			tmp = safe_emalloc(3 * rv, sizeof(char), 1);
			for (i=0;i<rv;i++) {
				sprintf(&tmp[3*sizeof(char)*i], " %02x", buf[i]);
			}
		} else {
			tmp = estrdup("");
		}
		php_gd_error_ex(E_NOTICE, "[GetDataBlock returning %d: %s]", rv, tmp);
		efree(tmp);
	}
	return(rv);
}