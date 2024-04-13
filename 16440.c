static int io_shutdown_prep(struct io_kiocb *req,
			    const struct io_uring_sqe *sqe)
{
#if defined(CONFIG_NET)
	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (unlikely(sqe->ioprio || sqe->off || sqe->addr || sqe->rw_flags ||
		     sqe->buf_index || sqe->splice_fd_in))
		return -EINVAL;

	req->shutdown.how = READ_ONCE(sqe->len);
	return 0;
#else
	return -EOPNOTSUPP;
#endif
}