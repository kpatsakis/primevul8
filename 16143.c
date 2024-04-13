static int io_close_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (sqe->ioprio || sqe->off || sqe->addr || sqe->len ||
	    sqe->rw_flags || sqe->buf_index)
		return -EINVAL;
	if (req->flags & REQ_F_FIXED_FILE)
		return -EBADF;

	req->close.fd = READ_ONCE(sqe->fd);
	req->close.file_slot = READ_ONCE(sqe->file_index);
	if (req->close.file_slot && req->close.fd)
		return -EINVAL;

	return 0;
}