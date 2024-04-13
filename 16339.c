static int io_connect_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_connect *conn = &req->connect;

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (sqe->ioprio || sqe->len || sqe->buf_index || sqe->rw_flags ||
	    sqe->splice_fd_in)
		return -EINVAL;

	conn->addr = u64_to_user_ptr(READ_ONCE(sqe->addr));
	conn->addr_len =  READ_ONCE(sqe->addr2);
	return 0;
}