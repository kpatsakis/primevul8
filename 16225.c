static int io_remove_buffers(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_provide_buf *p = &req->pbuf;
	struct io_ring_ctx *ctx = req->ctx;
	struct io_buffer_list *bl;
	int ret = 0;
	bool needs_lock = issue_flags & IO_URING_F_UNLOCKED;

	io_ring_submit_lock(ctx, needs_lock);

	lockdep_assert_held(&ctx->uring_lock);

	ret = -ENOENT;
	bl = io_buffer_get_list(ctx, p->bgid);
	if (bl)
		ret = __io_remove_buffers(ctx, bl, p->nbufs);
	if (ret < 0)
		req_set_fail(req);

	/* complete before unlock, IOPOLL may need the lock */
	__io_req_complete(req, issue_flags, ret, 0);
	io_ring_submit_unlock(ctx, needs_lock);
	return 0;
}