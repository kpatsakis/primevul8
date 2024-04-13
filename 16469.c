static int io_linkat(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_hardlink *lnk = &req->hardlink;
	int ret;

	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	ret = do_linkat(lnk->old_dfd, lnk->oldpath, lnk->new_dfd,
				lnk->newpath, lnk->flags);

	req->flags &= ~REQ_F_NEED_CLEANUP;
	if (ret < 0)
		req_set_fail(req);
	io_req_complete(req, ret);
	return 0;
}