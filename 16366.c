static bool io_resubmit_prep(struct io_kiocb *req)
{
	struct io_async_rw *rw = req->async_data;

	if (!req_has_async_data(req))
		return !io_req_prep_async(req);
	iov_iter_restore(&rw->s.iter, &rw->s.iter_state);
	return true;
}