static int io_mkdirat_prep(struct io_kiocb *req,
			    const struct io_uring_sqe *sqe)
{
	struct io_mkdir *mkd = &req->mkdir;
	const char __user *fname;

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (sqe->ioprio || sqe->off || sqe->rw_flags || sqe->buf_index ||
	    sqe->splice_fd_in)
		return -EINVAL;
	if (unlikely(req->flags & REQ_F_FIXED_FILE))
		return -EBADF;

	mkd->dfd = READ_ONCE(sqe->fd);
	mkd->mode = READ_ONCE(sqe->len);

	fname = u64_to_user_ptr(READ_ONCE(sqe->addr));
	mkd->filename = getname(fname);
	if (IS_ERR(mkd->filename))
		return PTR_ERR(mkd->filename);

	req->flags |= REQ_F_NEED_CLEANUP;
	return 0;
}