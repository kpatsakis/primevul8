static int io_accept(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_accept *accept = &req->accept;
	bool force_nonblock = issue_flags & IO_URING_F_NONBLOCK;
	unsigned int file_flags = force_nonblock ? O_NONBLOCK : 0;
	bool fixed = !!accept->file_slot;
	struct file *file;
	int ret, fd;

	if (!fixed) {
		fd = __get_unused_fd_flags(accept->flags, accept->nofile);
		if (unlikely(fd < 0))
			return fd;
	}
	file = do_accept(req->file, file_flags, accept->addr, accept->addr_len,
			 accept->flags);
	if (IS_ERR(file)) {
		if (!fixed)
			put_unused_fd(fd);
		ret = PTR_ERR(file);
		if (ret == -EAGAIN && force_nonblock)
			return -EAGAIN;
		if (ret == -ERESTARTSYS)
			ret = -EINTR;
		req_set_fail(req);
	} else if (!fixed) {
		fd_install(fd, file);
		ret = fd;
	} else {
		ret = io_install_fixed_file(req, file, issue_flags,
					    accept->file_slot - 1);
	}
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
}