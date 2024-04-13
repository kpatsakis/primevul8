
static void io_poll_cancel_req(struct io_kiocb *req)
{
	io_poll_mark_cancelled(req);
	/* kick tw, which should complete the request */
	io_poll_execute(req, 0, 0);