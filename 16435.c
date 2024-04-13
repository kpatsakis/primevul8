static int io_write(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_rw_state __s, *s = &__s;
	struct iovec *iovec;
	struct kiocb *kiocb = &req->rw.kiocb;
	bool force_nonblock = issue_flags & IO_URING_F_NONBLOCK;
	ssize_t ret, ret2;
	loff_t *ppos;

	if (!req_has_async_data(req)) {
		ret = io_import_iovec(WRITE, req, &iovec, s, issue_flags);
		if (unlikely(ret < 0))
			return ret;
	} else {
		struct io_async_rw *rw = req->async_data;

		s = &rw->s;
		iov_iter_restore(&s->iter, &s->iter_state);
		iovec = NULL;
	}
	ret = io_rw_init_file(req, FMODE_WRITE);
	if (unlikely(ret)) {
		kfree(iovec);
		return ret;
	}
	req->result = iov_iter_count(&s->iter);

	if (force_nonblock) {
		/* If the file doesn't support async, just async punt */
		if (unlikely(!io_file_supports_nowait(req)))
			goto copy_iov;

		/* file path doesn't support NOWAIT for non-direct_IO */
		if (force_nonblock && !(kiocb->ki_flags & IOCB_DIRECT) &&
		    (req->flags & REQ_F_ISREG))
			goto copy_iov;

		kiocb->ki_flags |= IOCB_NOWAIT;
	} else {
		/* Ensure we clear previously set non-block flag */
		kiocb->ki_flags &= ~IOCB_NOWAIT;
	}

	ppos = io_kiocb_update_pos(req);

	ret = rw_verify_area(WRITE, req->file, ppos, req->result);
	if (unlikely(ret))
		goto out_free;

	/*
	 * Open-code file_start_write here to grab freeze protection,
	 * which will be released by another thread in
	 * io_complete_rw().  Fool lockdep by telling it the lock got
	 * released so that it doesn't complain about the held lock when
	 * we return to userspace.
	 */
	if (req->flags & REQ_F_ISREG) {
		sb_start_write(file_inode(req->file)->i_sb);
		__sb_writers_release(file_inode(req->file)->i_sb,
					SB_FREEZE_WRITE);
	}
	kiocb->ki_flags |= IOCB_WRITE;

	if (likely(req->file->f_op->write_iter))
		ret2 = call_write_iter(req->file, kiocb, &s->iter);
	else if (req->file->f_op->write)
		ret2 = loop_rw_iter(WRITE, req, &s->iter);
	else
		ret2 = -EINVAL;

	if (req->flags & REQ_F_REISSUE) {
		req->flags &= ~REQ_F_REISSUE;
		ret2 = -EAGAIN;
	}

	/*
	 * Raw bdev writes will return -EOPNOTSUPP for IOCB_NOWAIT. Just
	 * retry them without IOCB_NOWAIT.
	 */
	if (ret2 == -EOPNOTSUPP && (kiocb->ki_flags & IOCB_NOWAIT))
		ret2 = -EAGAIN;
	/* no retry on NONBLOCK nor RWF_NOWAIT */
	if (ret2 == -EAGAIN && (req->flags & REQ_F_NOWAIT))
		goto done;
	if (!force_nonblock || ret2 != -EAGAIN) {
		/* IOPOLL retry should happen for io-wq threads */
		if (ret2 == -EAGAIN && (req->ctx->flags & IORING_SETUP_IOPOLL))
			goto copy_iov;
done:
		kiocb_done(req, ret2, issue_flags);
	} else {
copy_iov:
		iov_iter_restore(&s->iter, &s->iter_state);
		ret = io_setup_async_rw(req, iovec, s, false);
		return ret ?: -EAGAIN;
	}
out_free:
	/* it's reportedly faster than delegating the null check to kfree() */
	if (iovec)
		kfree(iovec);
	return ret;
}