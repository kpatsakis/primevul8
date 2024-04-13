
static int io_async_cancel_prep(struct io_kiocb *req,
				const struct io_uring_sqe *sqe)
{
	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (unlikely(req->flags & (REQ_F_FIXED_FILE | REQ_F_BUFFER_SELECT)))
		return -EINVAL;
	if (sqe->ioprio || sqe->off || sqe->len || sqe->cancel_flags ||
	    sqe->splice_fd_in)
		return -EINVAL;

	req->cancel.addr = READ_ONCE(sqe->addr);
	return 0;