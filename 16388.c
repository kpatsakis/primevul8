static __cold void __io_free_req(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;

	io_req_put_rsrc(req, ctx);
	io_dismantle_req(req);
	io_put_task(req->task, 1);

	spin_lock(&ctx->completion_lock);
	wq_list_add_head(&req->comp_list, &ctx->locked_free_list);
	ctx->locked_free_nr++;
	spin_unlock(&ctx->completion_lock);
}