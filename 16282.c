static inline int io_iter_do_read(struct io_kiocb *req, struct iov_iter *iter)
{
	if (likely(req->file->f_op->read_iter))
		return call_read_iter(req->file, &req->rw.kiocb, iter);
	else if (req->file->f_op->read)
		return loop_rw_iter(READ, req, iter);
	else
		return -EINVAL;
}