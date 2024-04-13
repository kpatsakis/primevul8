static void io_queue_sqe_arm_apoll(struct io_kiocb *req)
	__must_hold(&req->ctx->uring_lock)
{
	struct io_kiocb *linked_timeout = io_prep_linked_timeout(req);

	switch (io_arm_poll_handler(req, 0)) {
	case IO_APOLL_READY:
		io_req_task_queue(req);
		break;
	case IO_APOLL_ABORTED:
		/*
		 * Queued up for async execution, worker will release
		 * submit reference when the iocb is actually submitted.
		 */
		io_queue_async_work(req, NULL);
		break;
	case IO_APOLL_OK:
		break;
	}

	if (linked_timeout)
		io_queue_linked_timeout(linked_timeout);