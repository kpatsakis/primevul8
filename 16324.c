
static int io_async_cancel(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_ring_ctx *ctx = req->ctx;
	u64 sqe_addr = req->cancel.addr;
	bool needs_lock = issue_flags & IO_URING_F_UNLOCKED;
	struct io_tctx_node *node;
	int ret;

	ret = io_try_cancel_userdata(req, sqe_addr);
	if (ret != -ENOENT)
		goto done;

	/* slow path, try all io-wq's */
	io_ring_submit_lock(ctx, needs_lock);
	ret = -ENOENT;
	list_for_each_entry(node, &ctx->tctx_list, ctx_node) {
		struct io_uring_task *tctx = node->task->io_uring;

		ret = io_async_cancel_one(tctx, req->cancel.addr, ctx);
		if (ret != -ENOENT)
			break;
	}
	io_ring_submit_unlock(ctx, needs_lock);
done:
	if (ret < 0)
		req_set_fail(req);
	io_req_complete_post(req, ret, 0);
	return 0;