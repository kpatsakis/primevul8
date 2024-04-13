
static __cold void io_drain_req(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_defer_entry *de;
	int ret;
	u32 seq = io_get_sequence(req);

	/* Still need defer if there is pending req in defer list. */
	spin_lock(&ctx->completion_lock);
	if (!req_need_defer(req, seq) && list_empty_careful(&ctx->defer_list)) {
		spin_unlock(&ctx->completion_lock);
queue:
		ctx->drain_active = false;
		io_req_task_queue(req);
		return;
	}
	spin_unlock(&ctx->completion_lock);

	ret = io_req_prep_async(req);
	if (ret) {
fail:
		io_req_complete_failed(req, ret);
		return;
	}
	io_prep_async_link(req);
	de = kmalloc(sizeof(*de), GFP_KERNEL);
	if (!de) {
		ret = -ENOMEM;
		goto fail;
	}

	spin_lock(&ctx->completion_lock);
	if (!req_need_defer(req, seq) && list_empty(&ctx->defer_list)) {
		spin_unlock(&ctx->completion_lock);
		kfree(de);
		goto queue;
	}

	trace_io_uring_defer(ctx, req, req->user_data, req->opcode);
	de->req = req;
	de->seq = seq;
	list_add_tail(&de->list, &ctx->defer_list);
	spin_unlock(&ctx->completion_lock);