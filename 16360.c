static void __io_req_find_next_prep(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;
	bool posted;

	spin_lock(&ctx->completion_lock);
	posted = io_disarm_next(req);
	if (posted)
		io_commit_cqring(ctx);
	spin_unlock(&ctx->completion_lock);
	if (posted)
		io_cqring_ev_posted(ctx);
}