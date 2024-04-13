static ssize_t io_compat_import(struct io_kiocb *req, struct iovec *iov,
				unsigned int issue_flags)
{
	struct compat_iovec __user *uiov;
	compat_ssize_t clen;
	void __user *buf;
	ssize_t len;

	uiov = u64_to_user_ptr(req->rw.addr);
	if (!access_ok(uiov, sizeof(*uiov)))
		return -EFAULT;
	if (__get_user(clen, &uiov->iov_len))
		return -EFAULT;
	if (clen < 0)
		return -EINVAL;

	len = clen;
	buf = io_rw_buffer_select(req, &len, issue_flags);
	if (IS_ERR(buf))
		return PTR_ERR(buf);
	iov[0].iov_base = buf;
	iov[0].iov_len = (compat_size_t) len;
	return 0;
}