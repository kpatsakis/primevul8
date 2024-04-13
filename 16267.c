static void io_preinit_req(struct io_kiocb *req, struct io_ring_ctx *ctx)
{
	req->ctx = ctx;
	req->link = NULL;
	req->async_data = NULL;
	/* not necessary, but safer to zero */
	req->result = 0;
}