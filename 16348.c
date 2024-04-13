static int io_statx_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	const char __user *path;

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (sqe->ioprio || sqe->buf_index || sqe->splice_fd_in)
		return -EINVAL;
	if (req->flags & REQ_F_FIXED_FILE)
		return -EBADF;

	req->statx.dfd = READ_ONCE(sqe->fd);
	req->statx.mask = READ_ONCE(sqe->len);
	path = u64_to_user_ptr(READ_ONCE(sqe->addr));
	req->statx.buffer = u64_to_user_ptr(READ_ONCE(sqe->addr2));
	req->statx.flags = READ_ONCE(sqe->statx_flags);

	req->statx.filename = getname_flags(path,
					getname_statx_lookup_flags(req->statx.flags),
					NULL);

	if (IS_ERR(req->statx.filename)) {
		int ret = PTR_ERR(req->statx.filename);

		req->statx.filename = NULL;
		return ret;
	}

	req->flags |= REQ_F_NEED_CLEANUP;
	return 0;
}