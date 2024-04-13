ftp_raw(ftpbuf_t *ftp, const char *cmd, zval *return_value)
{
	if (ftp == NULL || cmd == NULL) {
		RETURN_NULL();
	}
	if (!ftp_putcmd(ftp, cmd, NULL)) {
		RETURN_NULL();
	}
	array_init(return_value);
	while (ftp_readline(ftp)) {
		add_next_index_string(return_value, ftp->inbuf, 1);
		if (isdigit(ftp->inbuf[0]) && isdigit(ftp->inbuf[1]) && isdigit(ftp->inbuf[2]) && ftp->inbuf[3] == ' ') {
			return;
		}
	}
}