
static void io_poll_mark_cancelled(struct io_kiocb *req)
{
	atomic_or(IO_POLL_CANCEL_FLAG, &req->poll_refs);