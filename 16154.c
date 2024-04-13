static int io_sfr_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_ring_ctx *ctx = req->ctx;

	if (unlikely(ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (unlikely(sqe->addr || sqe->ioprio || sqe->buf_index ||
		     sqe->splice_fd_in))
		return -EINVAL;

	req->sync.off = READ_ONCE(sqe->off);
	req->sync.len = READ_ONCE(sqe->len);
	req->sync.flags = READ_ONCE(sqe->sync_range_flags);
	return 0;
}