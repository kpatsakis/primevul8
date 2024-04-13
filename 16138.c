static int io_epoll_ctl(struct io_kiocb *req, unsigned int issue_flags)
{
#if defined(CONFIG_EPOLL)
	struct io_epoll *ie = &req->epoll;
	int ret;
	bool force_nonblock = issue_flags & IO_URING_F_NONBLOCK;

	ret = do_epoll_ctl(ie->epfd, ie->op, ie->fd, &ie->event, force_nonblock);
	if (force_nonblock && ret == -EAGAIN)
		return -EAGAIN;

	if (ret < 0)
		req_set_fail(req);
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
#else
	return -EOPNOTSUPP;
#endif
}