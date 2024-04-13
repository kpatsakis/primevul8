static int io_unlinkat(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_unlink *un = &req->unlink;
	int ret;

	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	if (un->flags & AT_REMOVEDIR)
		ret = do_rmdir(un->dfd, un->filename);
	else
		ret = do_unlinkat(un->dfd, un->filename);

	req->flags &= ~REQ_F_NEED_CLEANUP;
	if (ret < 0)
		req_set_fail(req);
	io_req_complete(req, ret);
	return 0;
}