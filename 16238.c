static int io_remove_buffers_prep(struct io_kiocb *req,
				  const struct io_uring_sqe *sqe)
{
	struct io_provide_buf *p = &req->pbuf;
	u64 tmp;

	if (sqe->ioprio || sqe->rw_flags || sqe->addr || sqe->len || sqe->off ||
	    sqe->splice_fd_in)
		return -EINVAL;

	tmp = READ_ONCE(sqe->fd);
	if (!tmp || tmp > USHRT_MAX)
		return -EINVAL;

	memset(p, 0, sizeof(*p));
	p->nbufs = tmp;
	p->bgid = READ_ONCE(sqe->buf_group);
	return 0;
}