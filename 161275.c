static long copy_from_user_iovec(void *buffer, const struct iovec *iov,
				 unsigned ioc)
{
	for (; ioc > 0; ioc--) {
		if (copy_from_user(buffer, iov->iov_base, iov->iov_len) != 0)
			return -EFAULT;
		buffer += iov->iov_len;
		iov++;
	}
	return 0;
}