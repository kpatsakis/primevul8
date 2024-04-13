
static void io_init_req_drain(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_kiocb *head = ctx->submit_state.link.head;

	ctx->drain_active = true;
	if (head) {
		/*
		 * If we need to drain a request in the middle of a link, drain
		 * the head request and the next request/link after the current
		 * link. Considering sequential execution of links,
		 * REQ_F_IO_DRAIN will be maintained for every request of our
		 * link.
		 */
		head->flags |= REQ_F_IO_DRAIN | REQ_F_FORCE_ASYNC;
		ctx->drain_next = true;
	}