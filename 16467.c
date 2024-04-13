
static u32 io_get_sequence(struct io_kiocb *req)
{
	u32 seq = req->ctx->cached_sq_head;

	/* need original cached_sq_head, but it was increased for each req */
	io_for_each_link(req, req)
		seq--;
	return seq;