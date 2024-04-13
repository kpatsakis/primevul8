my_accept(ftpbuf_t *ftp, php_socket_t s, struct sockaddr *addr, socklen_t *addrlen)
{
	int		n;

	n = php_pollfd_for_ms(s, PHP_POLLREADABLE, ftp->timeout_sec * 1000);
	if (n < 1) {
#if !defined(PHP_WIN32) && !(defined(NETWARE) && defined(USE_WINSOCK))
		if (n == 0) {
			errno = ETIMEDOUT;
		}
#endif
		return -1;
	}

	return accept(s, addr, addrlen);
}