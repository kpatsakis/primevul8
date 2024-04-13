static inline void __io_queue_sqe(struct io_kiocb *req)
	__must_hold(&req->ctx->uring_lock)
{
	struct io_kiocb *linked_timeout;
	int ret;

	ret = io_issue_sqe(req, IO_URING_F_NONBLOCK|IO_URING_F_COMPLETE_DEFER);

	if (req->flags & REQ_F_COMPLETE_INLINE) {
		io_req_add_compl_list(req);
		return;
	}
	/*
	 * We async punt it if the file wasn't marked NOWAIT, or if the file
	 * doesn't support non-blocking read/write attempts
	 */
	if (likely(!ret)) {
		linked_timeout = io_prep_linked_timeout(req);
		if (linked_timeout)
			io_queue_linked_timeout(linked_timeout);
	} else if (ret == -EAGAIN && !(req->flags & REQ_F_NOWAIT)) {
		io_queue_sqe_arm_apoll(req);
	} else {
		io_req_complete_failed(req, ret);
	}