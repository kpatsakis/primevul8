static int io_renameat(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_rename *ren = &req->rename;
	int ret;

	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	ret = do_renameat2(ren->old_dfd, ren->oldpath, ren->new_dfd,
				ren->newpath, ren->flags);

	req->flags &= ~REQ_F_NEED_CLEANUP;
	if (ret < 0)
		req_set_fail(req);
	io_req_complete(req, ret);
	return 0;
}