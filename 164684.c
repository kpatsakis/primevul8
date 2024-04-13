data_writeable(ftpbuf_t *ftp, php_socket_t s)
{
	int		n;

	n = php_pollfd_for_ms(s, POLLOUT, 1000);
	if (n < 1) {
#ifndef PHP_WIN32
		if (n == 0) {
			errno = ETIMEDOUT;
		}
#endif
		return 0;
	}

	return 1;
}