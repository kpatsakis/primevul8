static void io_queue_async_work(struct io_kiocb *req, bool *dont_use)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_kiocb *link = io_prep_linked_timeout(req);
	struct io_uring_task *tctx = req->task->io_uring;

	BUG_ON(!tctx);
	BUG_ON(!tctx->io_wq);

	/* init ->work of the whole link before punting */
	io_prep_async_link(req);

	/*
	 * Not expected to happen, but if we do have a bug where this _can_
	 * happen, catch it here and ensure the request is marked as
	 * canceled. That will make io-wq go through the usual work cancel
	 * procedure rather than attempt to run this request (or create a new
	 * worker for it).
	 */
	if (WARN_ON_ONCE(!same_thread_group(req->task, current)))
		req->work.flags |= IO_WQ_WORK_CANCEL;

	trace_io_uring_queue_async_work(ctx, req, req->user_data, req->opcode, req->flags,
					&req->work, io_wq_is_hashed(&req->work));
	io_wq_enqueue(tctx->io_wq, &req->work);
	if (link)
		io_queue_linked_timeout(link);
}