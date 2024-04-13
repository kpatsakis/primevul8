static int io_madvise_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
#if defined(CONFIG_ADVISE_SYSCALLS) && defined(CONFIG_MMU)
	if (sqe->ioprio || sqe->buf_index || sqe->off || sqe->splice_fd_in)
		return -EINVAL;
	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;

	req->madvise.addr = READ_ONCE(sqe->addr);
	req->madvise.len = READ_ONCE(sqe->len);
	req->madvise.advice = READ_ONCE(sqe->fadvise_advice);
	return 0;
#else
	return -EOPNOTSUPP;
#endif
}