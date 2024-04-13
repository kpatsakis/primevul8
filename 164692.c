ftp_nb_continue_read(ftpbuf_t *ftp TSRMLS_DC)
{
	databuf_t	*data = NULL;
	char		*ptr;
	int		lastch;
	size_t		rcvd;
	ftptype_t	type;

	data = ftp->data;

	/* check if there is already more data */
	if (!data_available(ftp, data->fd)) {
		return PHP_FTP_MOREDATA;
	}

	type = ftp->type;

	lastch = ftp->lastch;
	if ((rcvd = my_recv(ftp, data->fd, data->buf, FTP_BUFSIZE))) {
		if (rcvd == -1) {
			goto bail;
		}

		if (type == FTPTYPE_ASCII) {
			for (ptr = data->buf; rcvd; rcvd--, ptr++) {
				if (lastch == '\r' && *ptr != '\n') {
					php_stream_putc(ftp->stream, '\r');
				}
				if (*ptr != '\r') {
					php_stream_putc(ftp->stream, *ptr);
				}
				lastch = *ptr;
			}
		} else if (rcvd != php_stream_write(ftp->stream, data->buf, rcvd)) {
			goto bail;
		}

		ftp->lastch = lastch;
		return PHP_FTP_MOREDATA;
	}

	if (type == FTPTYPE_ASCII && lastch == '\r') {
		php_stream_putc(ftp->stream, '\r');
	}

	ftp->data = data = data_close(ftp, data);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250)) {
		goto bail;
	}

	ftp->nb = 0;
	return PHP_FTP_FINISHED;
bail:
	ftp->nb = 0;
	ftp->data = data_close(ftp, data);
	return PHP_FTP_FAILED;
}