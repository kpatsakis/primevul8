static inline int io_rw_prep_async(struct io_kiocb *req, int rw)
{
	struct io_async_rw *iorw = req->async_data;
	struct iovec *iov;
	int ret;

	/* submission path, ->uring_lock should already be taken */
	ret = io_import_iovec(rw, req, &iov, &iorw->s, 0);
	if (unlikely(ret < 0))
		return ret;

	iorw->bytes_done = 0;
	iorw->free_iovec = iov;
	if (iov)
		req->flags |= REQ_F_NEED_CLEANUP;
	return 0;
}