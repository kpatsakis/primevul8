static ssize_t __io_iov_buffer_select(struct io_kiocb *req, struct iovec *iov,
				      unsigned int issue_flags)
{
	struct iovec __user *uiov = u64_to_user_ptr(req->rw.addr);
	void __user *buf;
	ssize_t len;

	if (copy_from_user(iov, uiov, sizeof(*uiov)))
		return -EFAULT;

	len = iov[0].iov_len;
	if (len < 0)
		return -EINVAL;
	buf = io_rw_buffer_select(req, &len, issue_flags);
	if (IS_ERR(buf))
		return PTR_ERR(buf);
	iov[0].iov_base = buf;
	iov[0].iov_len = len;
	return 0;
}