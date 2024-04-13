static ssize_t loop_rw_iter(int rw, struct io_kiocb *req, struct iov_iter *iter)
{
	struct kiocb *kiocb = &req->rw.kiocb;
	struct file *file = req->file;
	ssize_t ret = 0;
	loff_t *ppos;

	/*
	 * Don't support polled IO through this interface, and we can't
	 * support non-blocking either. For the latter, this just causes
	 * the kiocb to be handled from an async context.
	 */
	if (kiocb->ki_flags & IOCB_HIPRI)
		return -EOPNOTSUPP;
	if ((kiocb->ki_flags & IOCB_NOWAIT) &&
	    !(kiocb->ki_filp->f_flags & O_NONBLOCK))
		return -EAGAIN;

	ppos = io_kiocb_ppos(kiocb);

	while (iov_iter_count(iter)) {
		struct iovec iovec;
		ssize_t nr;

		if (!iov_iter_is_bvec(iter)) {
			iovec = iov_iter_iovec(iter);
		} else {
			iovec.iov_base = u64_to_user_ptr(req->rw.addr);
			iovec.iov_len = req->rw.len;
		}

		if (rw == READ) {
			nr = file->f_op->read(file, iovec.iov_base,
					      iovec.iov_len, ppos);
		} else {
			nr = file->f_op->write(file, iovec.iov_base,
					       iovec.iov_len, ppos);
		}

		if (nr < 0) {
			if (!ret)
				ret = nr;
			break;
		}
		ret += nr;
		if (!iov_iter_is_bvec(iter)) {
			iov_iter_advance(iter, nr);
		} else {
			req->rw.addr += nr;
			req->rw.len -= nr;
			if (!req->rw.len)
				break;
		}
		if (nr != iovec.iov_len)
			break;
	}

	return ret;
}