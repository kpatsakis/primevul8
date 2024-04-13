static int io_fsync(struct io_kiocb *req, unsigned int issue_flags)
{
	loff_t end = req->sync.off + req->sync.len;
	int ret;

	/* fsync always requires a blocking context */
	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	ret = vfs_fsync_range(req->file, req->sync.off,
				end > 0 ? end : LLONG_MAX,
				req->sync.flags & IORING_FSYNC_DATASYNC);
	if (ret < 0)
		req_set_fail(req);
	io_req_complete(req, ret);
	return 0;
}