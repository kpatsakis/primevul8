static int io_fadvise_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	if (sqe->ioprio || sqe->buf_index || sqe->addr || sqe->splice_fd_in)
		return -EINVAL;
	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;

	req->fadvise.offset = READ_ONCE(sqe->off);
	req->fadvise.len = READ_ONCE(sqe->len);
	req->fadvise.advice = READ_ONCE(sqe->fadvise_advice);
	return 0;
}