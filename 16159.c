static void io_req_complete_post(struct io_kiocb *req, s32 res,
				 u32 cflags)
{
	struct io_ring_ctx *ctx = req->ctx;

	spin_lock(&ctx->completion_lock);
	__io_req_complete_post(req, res, cflags);
	io_commit_cqring(ctx);
	spin_unlock(&ctx->completion_lock);
	io_cqring_ev_posted(ctx);
}