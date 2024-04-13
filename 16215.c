static int io_fallocate_prep(struct io_kiocb *req,
			     const struct io_uring_sqe *sqe)
{
	if (sqe->ioprio || sqe->buf_index || sqe->rw_flags ||
	    sqe->splice_fd_in)
		return -EINVAL;
	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;

	req->sync.off = READ_ONCE(sqe->off);
	req->sync.len = READ_ONCE(sqe->addr);
	req->sync.mode = READ_ONCE(sqe->len);
	return 0;
}