static int io_mkdirat(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_mkdir *mkd = &req->mkdir;
	int ret;

	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	ret = do_mkdirat(mkd->dfd, mkd->filename, mkd->mode);

	req->flags &= ~REQ_F_NEED_CLEANUP;
	if (ret < 0)
		req_set_fail(req);
	io_req_complete(req, ret);
	return 0;
}