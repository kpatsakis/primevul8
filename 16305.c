static ssize_t io_iov_buffer_select(struct io_kiocb *req, struct iovec *iov,
				    unsigned int issue_flags)
{
	if (req->flags & REQ_F_BUFFER_SELECTED) {
		struct io_buffer *kbuf = req->kbuf;

		iov[0].iov_base = u64_to_user_ptr(kbuf->addr);
		iov[0].iov_len = kbuf->len;
		return 0;
	}
	if (req->rw.len != 1)
		return -EINVAL;

#ifdef CONFIG_COMPAT
	if (req->ctx->compat)
		return io_compat_import(req, iov, issue_flags);
#endif

	return __io_iov_buffer_select(req, iov, issue_flags);
}