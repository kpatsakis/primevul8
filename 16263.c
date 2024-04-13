static int io_msg_ring_prep(struct io_kiocb *req,
			    const struct io_uring_sqe *sqe)
{
	if (unlikely(sqe->addr || sqe->ioprio || sqe->rw_flags ||
		     sqe->splice_fd_in || sqe->buf_index || sqe->personality))
		return -EINVAL;

	req->msg.user_data = READ_ONCE(sqe->off);
	req->msg.len = READ_ONCE(sqe->len);
	return 0;
}