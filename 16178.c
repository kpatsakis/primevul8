static int io_linkat_prep(struct io_kiocb *req,
			    const struct io_uring_sqe *sqe)
{
	struct io_hardlink *lnk = &req->hardlink;
	const char __user *oldf, *newf;

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (sqe->ioprio || sqe->rw_flags || sqe->buf_index || sqe->splice_fd_in)
		return -EINVAL;
	if (unlikely(req->flags & REQ_F_FIXED_FILE))
		return -EBADF;

	lnk->old_dfd = READ_ONCE(sqe->fd);
	lnk->new_dfd = READ_ONCE(sqe->len);
	oldf = u64_to_user_ptr(READ_ONCE(sqe->addr));
	newf = u64_to_user_ptr(READ_ONCE(sqe->addr2));
	lnk->flags = READ_ONCE(sqe->hardlink_flags);

	lnk->oldpath = getname(oldf);
	if (IS_ERR(lnk->oldpath))
		return PTR_ERR(lnk->oldpath);

	lnk->newpath = getname(newf);
	if (IS_ERR(lnk->newpath)) {
		putname(lnk->oldpath);
		return PTR_ERR(lnk->newpath);
	}

	req->flags |= REQ_F_NEED_CLEANUP;
	return 0;
}