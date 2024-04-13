static void __io_complete_rw(struct io_kiocb *req, long res,
			     unsigned int issue_flags)
{
	if (__io_complete_rw_common(req, res))
		return;
	__io_req_complete(req, issue_flags, req->result,
				io_put_kbuf(req, issue_flags));
}