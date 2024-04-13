static int io_symlinkat(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_symlink *sl = &req->symlink;
	int ret;

	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	ret = do_symlinkat(sl->oldpath, sl->new_dfd, sl->newpath);

	req->flags &= ~REQ_F_NEED_CLEANUP;
	if (ret < 0)
		req_set_fail(req);
	io_req_complete(req, ret);
	return 0;
}