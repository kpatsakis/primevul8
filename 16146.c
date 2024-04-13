
static int io_try_cancel_userdata(struct io_kiocb *req, u64 sqe_addr)
{
	struct io_ring_ctx *ctx = req->ctx;
	int ret;

	WARN_ON_ONCE(!io_wq_current_is_worker() && req->task != current);

	ret = io_async_cancel_one(req->task->io_uring, sqe_addr, ctx);
	/*
	 * Fall-through even for -EALREADY, as we may have poll armed
	 * that need unarming.
	 */
	if (!ret)
		return 0;

	spin_lock(&ctx->completion_lock);
	ret = io_poll_cancel(ctx, sqe_addr, false);
	if (ret != -ENOENT)
		goto out;

	spin_lock_irq(&ctx->timeout_lock);
	ret = io_timeout_cancel(ctx, sqe_addr);
	spin_unlock_irq(&ctx->timeout_lock);
out:
	spin_unlock(&ctx->completion_lock);
	return ret;