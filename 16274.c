
static void io_poll_task_func(struct io_kiocb *req, bool *locked)
{
	struct io_ring_ctx *ctx = req->ctx;
	int ret;

	ret = io_poll_check_events(req, *locked);
	if (ret > 0)
		return;

	if (!ret) {
		req->result = mangle_poll(req->result & req->poll.events);
	} else {
		req->result = ret;
		req_set_fail(req);
	}

	io_poll_remove_entries(req);
	spin_lock(&ctx->completion_lock);
	hash_del(&req->hash_node);
	__io_req_complete_post(req, req->result, 0);
	io_commit_cqring(ctx);
	spin_unlock(&ctx->completion_lock);
	io_cqring_ev_posted(ctx);