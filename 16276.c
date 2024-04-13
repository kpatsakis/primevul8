static void io_queue_sqe_fallback(struct io_kiocb *req)
	__must_hold(&req->ctx->uring_lock)
{
	if (req->flags & REQ_F_FAIL) {
		io_req_complete_fail_submit(req);
	} else if (unlikely(req->ctx->drain_active)) {
		io_drain_req(req);
	} else {
		int ret = io_req_prep_async(req);

		if (unlikely(ret))
			io_req_complete_failed(req, ret);
		else
			io_queue_async_work(req, NULL);
	}