static int io_splice_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_splice *sp = &req->splice;

	sp->off_in = READ_ONCE(sqe->splice_off_in);
	sp->off_out = READ_ONCE(sqe->off);
	return __io_splice_prep(req, sqe);
}