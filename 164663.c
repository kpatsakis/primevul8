ftp_getdata(ftpbuf_t *ftp TSRMLS_DC)
{
	int			fd = -1;
	databuf_t		*data;
	php_sockaddr_storage addr;
	struct sockaddr *sa;
	socklen_t		size;
	union ipbox		ipbox;
	char			arg[sizeof("255, 255, 255, 255, 255, 255")];
	struct timeval	tv;


	/* ask for a passive connection if we need one */
	if (ftp->pasv && !ftp_pasv(ftp, 1)) {
		return NULL;
	}
	/* alloc the data structure */
	data = ecalloc(1, sizeof(*data));
	data->listener = -1;
	data->fd = -1;
	data->type = ftp->type;

	sa = (struct sockaddr *) &ftp->localaddr;
	/* bind/listen */
	if ((fd = socket(sa->sa_family, SOCK_STREAM, 0)) == SOCK_ERR) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "socket() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	/* passive connection handler */
	if (ftp->pasv) {
		/* clear the ready status */
		ftp->pasv = 1;

		/* connect */
		/* Win 95/98 seems not to like size > sizeof(sockaddr_in) */
		size = php_sockaddr_size(&ftp->pasvaddr);
		tv.tv_sec = ftp->timeout_sec;
		tv.tv_usec = 0;
		if (php_connect_nonb(fd, (struct sockaddr*) &ftp->pasvaddr, size, &tv) == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_connect_nonb() failed: %s (%d)", strerror(errno), errno);
			goto bail;
		}

		data->fd = fd;

		ftp->data = data;
		return data;
	}


	/* active (normal) connection */

	/* bind to a local address */
	php_any_addr(sa->sa_family, &addr, 0);
	size = php_sockaddr_size(&addr);

	if (bind(fd, (struct sockaddr*) &addr, size) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "bind() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	if (getsockname(fd, (struct sockaddr*) &addr, &size) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "getsockname() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	if (listen(fd, 5) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "listen() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	data->listener = fd;

#if HAVE_IPV6 && HAVE_INET_NTOP
	if (sa->sa_family == AF_INET6) {
		/* need to use EPRT */
		char eprtarg[INET6_ADDRSTRLEN + sizeof("|x||xxxxx|")];
		char out[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &((struct sockaddr_in6*) sa)->sin6_addr, out, sizeof(out));
		snprintf(eprtarg, sizeof(eprtarg), "|2|%s|%hu|", out, ntohs(((struct sockaddr_in6 *) &addr)->sin6_port));

		if (!ftp_putcmd(ftp, "EPRT", eprtarg)) {
			goto bail;
		}

		if (!ftp_getresp(ftp) || ftp->resp != 200) {
			goto bail;
		}

		ftp->data = data;
		return data;
	}
#endif

	/* send the PORT */
	ipbox.ia[0] = ((struct sockaddr_in*) sa)->sin_addr;
	ipbox.s[2] = ((struct sockaddr_in*) &addr)->sin_port;
	snprintf(arg, sizeof(arg), "%u,%u,%u,%u,%u,%u", ipbox.c[0], ipbox.c[1], ipbox.c[2], ipbox.c[3], ipbox.c[4], ipbox.c[5]);

	if (!ftp_putcmd(ftp, "PORT", arg)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 200) {
		goto bail;
	}

	ftp->data = data;
	return data;

bail:
	if (fd != -1) {
		closesocket(fd);
	}
	efree(data);
	return NULL;
}