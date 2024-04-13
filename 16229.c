
static void io_wq_submit_work(struct io_wq_work *work)
{
	struct io_kiocb *req = container_of(work, struct io_kiocb, work);
	const struct io_op_def *def = &io_op_defs[req->opcode];
	unsigned int issue_flags = IO_URING_F_UNLOCKED;
	bool needs_poll = false;
	struct io_kiocb *timeout;
	int ret = 0, err = -ECANCELED;

	/* one will be dropped by ->io_free_work() after returning to io-wq */
	if (!(req->flags & REQ_F_REFCOUNT))
		__io_req_set_refcount(req, 2);
	else
		req_ref_get(req);

	timeout = io_prep_linked_timeout(req);
	if (timeout)
		io_queue_linked_timeout(timeout);


	/* either cancelled or io-wq is dying, so don't touch tctx->iowq */
	if (work->flags & IO_WQ_WORK_CANCEL) {
fail:
		io_req_task_queue_fail(req, err);
		return;
	}
	if (!io_assign_file(req, issue_flags)) {
		err = -EBADF;
		work->flags |= IO_WQ_WORK_CANCEL;
		goto fail;
	}

	if (req->flags & REQ_F_FORCE_ASYNC) {
		bool opcode_poll = def->pollin || def->pollout;

		if (opcode_poll && file_can_poll(req->file)) {
			needs_poll = true;
			issue_flags |= IO_URING_F_NONBLOCK;
		}
	}

	do {
		ret = io_issue_sqe(req, issue_flags);
		if (ret != -EAGAIN)
			break;
		/*
		 * We can get EAGAIN for iopolled IO even though we're
		 * forcing a sync submission from here, since we can't
		 * wait for request slots on the block side.
		 */
		if (!needs_poll) {
			cond_resched();
			continue;
		}

		if (io_arm_poll_handler(req, issue_flags) == IO_APOLL_OK)
			return;
		/* aborted or ready, in either case retry blocking */
		needs_poll = false;
		issue_flags &= ~IO_URING_F_NONBLOCK;
	} while (1);

	/* avoid locking problems by failing it from a clean context */
	if (ret)
		io_req_task_queue_fail(req, ret);