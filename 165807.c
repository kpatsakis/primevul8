ssize_t sys_writev(int fd, const struct iovec *iov, int iovcnt)
{
	ssize_t ret;

#if 0
	/* Try to confuse write_data_iov a bit */
	if ((random() % 5) == 0) {
		return sys_write(fd, iov[0].iov_base, iov[0].iov_len);
	}
	if (iov[0].iov_len > 1) {
		return sys_write(fd, iov[0].iov_base,
				 (random() % (iov[0].iov_len-1)) + 1);
	}
#endif

	do {
		ret = writev(fd, iov, iovcnt);
#if defined(EWOULDBLOCK)
	} while (ret == -1 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK));
#else
	} while (ret == -1 && (errno == EINTR || errno == EAGAIN));