 */
static int io_timeout_remove(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_timeout_rem *tr = &req->timeout_rem;
	struct io_ring_ctx *ctx = req->ctx;
	int ret;

	if (!(req->timeout_rem.flags & IORING_TIMEOUT_UPDATE)) {
		spin_lock(&ctx->completion_lock);
		spin_lock_irq(&ctx->timeout_lock);
		ret = io_timeout_cancel(ctx, tr->addr);
		spin_unlock_irq(&ctx->timeout_lock);
		spin_unlock(&ctx->completion_lock);
	} else {
		enum hrtimer_mode mode = io_translate_timeout_mode(tr->flags);

		spin_lock_irq(&ctx->timeout_lock);
		if (tr->ltimeout)
			ret = io_linked_timeout_update(ctx, tr->addr, &tr->ts, mode);
		else
			ret = io_timeout_update(ctx, tr->addr, &tr->ts, mode);
		spin_unlock_irq(&ctx->timeout_lock);
	}

	if (ret < 0)
		req_set_fail(req);
	io_req_complete_post(req, ret, 0);
	return 0;