static inline void io_queue_sqe(struct io_kiocb *req)
	__must_hold(&req->ctx->uring_lock)
{
	if (likely(!(req->flags & (REQ_F_FORCE_ASYNC | REQ_F_FAIL))))
		__io_queue_sqe(req);
	else
		io_queue_sqe_fallback(req);