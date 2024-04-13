static int io_submit_sqes(struct io_ring_ctx *ctx, unsigned int nr)
	__must_hold(&ctx->uring_lock)
{
	unsigned int entries = io_sqring_entries(ctx);
	int submitted = 0;

	if (unlikely(!entries))
		return 0;
	/* make sure SQ entry isn't read before tail */
	nr = min3(nr, ctx->sq_entries, entries);
	io_get_task_refs(nr);

	io_submit_state_start(&ctx->submit_state, nr);
	do {
		const struct io_uring_sqe *sqe;
		struct io_kiocb *req;

		if (unlikely(!io_alloc_req_refill(ctx))) {
			if (!submitted)
				submitted = -EAGAIN;
			break;
		}
		req = io_alloc_req(ctx);
		sqe = io_get_sqe(ctx);
		if (unlikely(!sqe)) {
			wq_stack_add_head(&req->comp_list, &ctx->submit_state.free_list);
			break;
		}
		/* will complete beyond this point, count as submitted */
		submitted++;
		if (io_submit_sqe(ctx, req, sqe)) {
			/*
			 * Continue submitting even for sqe failure if the
			 * ring was setup with IORING_SETUP_SUBMIT_ALL
			 */
			if (!(ctx->flags & IORING_SETUP_SUBMIT_ALL))
				break;
		}
	} while (submitted < nr);

	if (unlikely(submitted != nr)) {
		int ref_used = (submitted == -EAGAIN) ? 0 : submitted;
		int unused = nr - ref_used;

		current->io_uring->cached_refs += unused;
	}

	io_submit_state_end(ctx);
	 /* Commit SQ ring head once we've consumed and submitted all SQEs */
	io_commit_sqring(ctx);

	return submitted;