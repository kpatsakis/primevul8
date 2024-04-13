
static int io_rsrc_update_prep(struct io_kiocb *req,
				const struct io_uring_sqe *sqe)
{
	if (unlikely(req->flags & (REQ_F_FIXED_FILE | REQ_F_BUFFER_SELECT)))
		return -EINVAL;
	if (sqe->ioprio || sqe->rw_flags || sqe->splice_fd_in)
		return -EINVAL;

	req->rsrc_update.offset = READ_ONCE(sqe->off);
	req->rsrc_update.nr_args = READ_ONCE(sqe->len);
	if (!req->rsrc_update.nr_args)
		return -EINVAL;
	req->rsrc_update.arg = READ_ONCE(sqe->addr);
	return 0;