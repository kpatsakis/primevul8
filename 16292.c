static int io_unlinkat_prep(struct io_kiocb *req,
			    const struct io_uring_sqe *sqe)
{
	struct io_unlink *un = &req->unlink;
	const char __user *fname;

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (sqe->ioprio || sqe->off || sqe->len || sqe->buf_index ||
	    sqe->splice_fd_in)
		return -EINVAL;
	if (unlikely(req->flags & REQ_F_FIXED_FILE))
		return -EBADF;

	un->dfd = READ_ONCE(sqe->fd);

	un->flags = READ_ONCE(sqe->unlink_flags);
	if (un->flags & ~AT_REMOVEDIR)
		return -EINVAL;

	fname = u64_to_user_ptr(READ_ONCE(sqe->addr));
	un->filename = getname(fname);
	if (IS_ERR(un->filename))
		return PTR_ERR(un->filename);

	req->flags |= REQ_F_NEED_CLEANUP;
	return 0;
}