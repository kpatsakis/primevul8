data_available(ftpbuf_t *ftp, php_socket_t s)
{
	int		n;

	n = php_pollfd_for_ms(s, PHP_POLLREADABLE, 1000);
	if (n < 1) {
#if !defined(PHP_WIN32) && !(defined(NETWARE) && defined(USE_WINSOCK))
		if (n == 0) {
			errno = ETIMEDOUT;
		}
#endif
		return 0;
	}

	return 1;
}