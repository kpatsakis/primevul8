ssize_t sys_read(int fd, void *buf, size_t count)
{
	ssize_t ret;

	do {
		ret = read(fd, buf, count);
#if defined(EWOULDBLOCK)
	} while (ret == -1 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK));
#else
	} while (ret == -1 && (errno == EINTR || errno == EAGAIN));