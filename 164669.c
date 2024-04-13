ftp_open(const char *host, short port, long timeout_sec TSRMLS_DC)
{
	ftpbuf_t		*ftp;
	socklen_t		 size;
	struct timeval tv;


	/* alloc the ftp structure */
	ftp = ecalloc(1, sizeof(*ftp));

	tv.tv_sec = timeout_sec;
	tv.tv_usec = 0;

	ftp->fd = php_network_connect_socket_to_host(host,
			(unsigned short) (port ? port : 21), SOCK_STREAM,
			0, &tv, NULL, NULL, NULL, 0 TSRMLS_CC);
	if (ftp->fd == -1) {
		goto bail;
	}

	/* Default Settings */
	ftp->timeout_sec = timeout_sec;
	ftp->nb = 0;

	size = sizeof(ftp->localaddr);
	memset(&ftp->localaddr, 0, size);
	if (getsockname(ftp->fd, (struct sockaddr*) &ftp->localaddr, &size) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "getsockname failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	if (!ftp_getresp(ftp) || ftp->resp != 220) {
		goto bail;
	}

	return ftp;

bail:
	if (ftp->fd != -1) {
		closesocket(ftp->fd);
	}
	efree(ftp);
	return NULL;
}