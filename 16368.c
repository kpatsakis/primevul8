static int io_shutdown(struct io_kiocb *req, unsigned int issue_flags)
{
#if defined(CONFIG_NET)
	struct socket *sock;
	int ret;

	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	sock = sock_from_file(req->file);
	if (unlikely(!sock))
		return -ENOTSOCK;

	ret = __sys_shutdown_sock(sock, req->shutdown.how);
	if (ret < 0)
		req_set_fail(req);
	io_req_complete(req, ret);
	return 0;
#else
	return -EOPNOTSUPP;
#endif
}