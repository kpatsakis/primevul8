static int io_symlinkat_prep(struct io_kiocb *req,
			    const struct io_uring_sqe *sqe)
{
	struct io_symlink *sl = &req->symlink;
	const char __user *oldpath, *newpath;

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (sqe->ioprio || sqe->len || sqe->rw_flags || sqe->buf_index ||
	    sqe->splice_fd_in)
		return -EINVAL;
	if (unlikely(req->flags & REQ_F_FIXED_FILE))
		return -EBADF;

	sl->new_dfd = READ_ONCE(sqe->fd);
	oldpath = u64_to_user_ptr(READ_ONCE(sqe->addr));
	newpath = u64_to_user_ptr(READ_ONCE(sqe->addr2));

	sl->oldpath = getname(oldpath);
	if (IS_ERR(sl->oldpath))
		return PTR_ERR(sl->oldpath);

	sl->newpath = getname(newpath);
	if (IS_ERR(sl->newpath)) {
		putname(sl->oldpath);
		return PTR_ERR(sl->newpath);
	}

	req->flags |= REQ_F_NEED_CLEANUP;
	return 0;
}