static int io_prep_rw(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct kiocb *kiocb = &req->rw.kiocb;
	unsigned ioprio;
	int ret;

	kiocb->ki_pos = READ_ONCE(sqe->off);

	ioprio = READ_ONCE(sqe->ioprio);
	if (ioprio) {
		ret = ioprio_check_cap(ioprio);
		if (ret)
			return ret;

		kiocb->ki_ioprio = ioprio;
	} else {
		kiocb->ki_ioprio = get_current_ioprio();
	}

	req->imu = NULL;
	req->rw.addr = READ_ONCE(sqe->addr);
	req->rw.len = READ_ONCE(sqe->len);
	req->rw.flags = READ_ONCE(sqe->rw_flags);
	req->buf_index = READ_ONCE(sqe->buf_index);
	return 0;
}