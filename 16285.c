static void io_req_task_submit(struct io_kiocb *req, bool *locked)
{
	struct io_ring_ctx *ctx = req->ctx;

	io_tw_lock(ctx, locked);
	/* req->task == current here, checking PF_EXITING is safe */
	if (likely(!(req->task->flags & PF_EXITING)))
		__io_queue_sqe(req);
	else
		io_req_complete_failed(req, -EFAULT);
}