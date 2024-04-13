ftp_nb_get(ftpbuf_t *ftp, php_stream *outstream, const char *path, ftptype_t type, long resumepos TSRMLS_DC)
{
	databuf_t		*data = NULL;
	char			arg[11];

	if (ftp == NULL) {
		return PHP_FTP_FAILED;
	}

	if (!ftp_type(ftp, type)) {
		goto bail;
	}

	if ((data = ftp_getdata(ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}

	if (resumepos>0) {
		snprintf(arg, sizeof(arg), "%ld", resumepos);
		if (!ftp_putcmd(ftp, "REST", arg)) {
			goto bail;
		}
		if (!ftp_getresp(ftp) || (ftp->resp != 350)) {
			goto bail;
		}
	}

	if (!ftp_putcmd(ftp, "RETR", path)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}

	if ((data = data_accept(data, ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}

	ftp->data = data;
	ftp->stream = outstream;
	ftp->lastch = 0;
	ftp->nb = 1;

	return (ftp_nb_continue_read(ftp TSRMLS_CC));

bail:
	ftp->data = data_close(ftp, data);
	return PHP_FTP_FAILED;
}