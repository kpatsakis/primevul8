
static void io_apoll_task_func(struct io_kiocb *req, bool *locked)
{
	struct io_ring_ctx *ctx = req->ctx;
	int ret;

	ret = io_poll_check_events(req, *locked);
	if (ret > 0)
		return;

	io_poll_remove_entries(req);
	spin_lock(&ctx->completion_lock);
	hash_del(&req->hash_node);
	spin_unlock(&ctx->completion_lock);

	if (!ret)
		io_req_task_submit(req, locked);
	else
		io_req_complete_failed(req, ret);