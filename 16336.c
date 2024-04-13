static int io_openat2(struct io_kiocb *req, unsigned int issue_flags)
{
	struct open_flags op;
	struct file *file;
	bool resolve_nonblock, nonblock_set;
	bool fixed = !!req->open.file_slot;
	int ret;

	ret = build_open_flags(&req->open.how, &op);
	if (ret)
		goto err;
	nonblock_set = op.open_flag & O_NONBLOCK;
	resolve_nonblock = req->open.how.resolve & RESOLVE_CACHED;
	if (issue_flags & IO_URING_F_NONBLOCK) {
		/*
		 * Don't bother trying for O_TRUNC, O_CREAT, or O_TMPFILE open,
		 * it'll always -EAGAIN
		 */
		if (req->open.how.flags & (O_TRUNC | O_CREAT | O_TMPFILE))
			return -EAGAIN;
		op.lookup_flags |= LOOKUP_CACHED;
		op.open_flag |= O_NONBLOCK;
	}

	if (!fixed) {
		ret = __get_unused_fd_flags(req->open.how.flags, req->open.nofile);
		if (ret < 0)
			goto err;
	}

	file = do_filp_open(req->open.dfd, req->open.filename, &op);
	if (IS_ERR(file)) {
		/*
		 * We could hang on to this 'fd' on retrying, but seems like
		 * marginal gain for something that is now known to be a slower
		 * path. So just put it, and we'll get a new one when we retry.
		 */
		if (!fixed)
			put_unused_fd(ret);

		ret = PTR_ERR(file);
		/* only retry if RESOLVE_CACHED wasn't already set by application */
		if (ret == -EAGAIN &&
		    (!resolve_nonblock && (issue_flags & IO_URING_F_NONBLOCK)))
			return -EAGAIN;
		goto err;
	}

	if ((issue_flags & IO_URING_F_NONBLOCK) && !nonblock_set)
		file->f_flags &= ~O_NONBLOCK;
	fsnotify_open(file);

	if (!fixed)
		fd_install(ret, file);
	else
		ret = io_install_fixed_file(req, file, issue_flags,
					    req->open.file_slot - 1);
err:
	putname(req->open.filename);
	req->flags &= ~REQ_F_NEED_CLEANUP;
	if (ret < 0)
		req_set_fail(req);
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
}