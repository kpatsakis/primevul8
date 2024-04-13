cdf_read(const cdf_info_t *info, off_t off, void *buf, size_t len)
{
	size_t siz = (size_t)off + len;

	if ((off_t)(off + len) != (off_t)siz)
		goto out;

	if (info->i_buf != NULL && info->i_len >= siz) {
		(void)memcpy(buf, &info->i_buf[off], len);
		return (ssize_t)len;
	}

	if (info->i_fd == -1)
		goto out;

	if (pread(info->i_fd, buf, len, off) != (ssize_t)len)
		return -1;

	return (ssize_t)len;
out:
	errno = EINVAL;
	return -1;
}