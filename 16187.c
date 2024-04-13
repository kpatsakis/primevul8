 */
static int io_poll_check_events(struct io_kiocb *req, bool locked)
{
	struct io_ring_ctx *ctx = req->ctx;
	int v;

	/* req->task == current here, checking PF_EXITING is safe */
	if (unlikely(req->task->flags & PF_EXITING))
		io_poll_mark_cancelled(req);

	do {
		v = atomic_read(&req->poll_refs);

		/* tw handler should be the owner, and so have some references */
		if (WARN_ON_ONCE(!(v & IO_POLL_REF_MASK)))
			return 0;
		if (v & IO_POLL_CANCEL_FLAG)
			return -ECANCELED;

		if (!req->result) {
			struct poll_table_struct pt = { ._key = req->apoll_events };
			unsigned flags = locked ? 0 : IO_URING_F_UNLOCKED;

			if (unlikely(!io_assign_file(req, flags)))
				return -EBADF;
			req->result = vfs_poll(req->file, &pt) & req->apoll_events;
		}

		/* multishot, just fill an CQE and proceed */
		if (req->result && !(req->apoll_events & EPOLLONESHOT)) {
			__poll_t mask = mangle_poll(req->result & req->apoll_events);
			bool filled;

			spin_lock(&ctx->completion_lock);
			filled = io_fill_cqe_aux(ctx, req->user_data, mask,
						 IORING_CQE_F_MORE);
			io_commit_cqring(ctx);
			spin_unlock(&ctx->completion_lock);
			if (unlikely(!filled))
				return -ECANCELED;
			io_cqring_ev_posted(ctx);
		} else if (req->result) {
			return 0;
		}

		/*
		 * Release all references, retry if someone tried to restart
		 * task_work while we were executing it.
		 */
	} while (atomic_sub_return(v & IO_POLL_REF_MASK, &req->poll_refs));

	return 1;