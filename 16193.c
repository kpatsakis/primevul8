static void io_req_task_cancel(struct io_kiocb *req, bool *locked)
{
	struct io_ring_ctx *ctx = req->ctx;

	/* not needed for normal modes, but SQPOLL depends on it */
	io_tw_lock(ctx, locked);
	io_req_complete_failed(req, req->result);
}