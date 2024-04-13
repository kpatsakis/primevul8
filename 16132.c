static bool __io_complete_rw_common(struct io_kiocb *req, long res)
{
	if (req->rw.kiocb.ki_flags & IOCB_WRITE) {
		kiocb_end_write(req);
		fsnotify_modify(req->file);
	} else {
		fsnotify_access(req->file);
	}
	if (unlikely(res != req->result)) {
		if ((res == -EAGAIN || res == -EOPNOTSUPP) &&
		    io_rw_should_reissue(req)) {
			req->flags |= REQ_F_REISSUE;
			return true;
		}
		req_set_fail(req);
		req->result = res;
	}
	return false;
}