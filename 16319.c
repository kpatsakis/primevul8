			 const struct io_uring_sqe *sqe)
	__must_hold(&ctx->uring_lock)
{
	struct io_submit_link *link = &ctx->submit_state.link;
	int ret;

	ret = io_init_req(ctx, req, sqe);
	if (unlikely(ret)) {
		trace_io_uring_req_failed(sqe, ctx, req, ret);

		/* fail even hard links since we don't submit */
		if (link->head) {
			/*
			 * we can judge a link req is failed or cancelled by if
			 * REQ_F_FAIL is set, but the head is an exception since
			 * it may be set REQ_F_FAIL because of other req's failure
			 * so let's leverage req->result to distinguish if a head
			 * is set REQ_F_FAIL because of its failure or other req's
			 * failure so that we can set the correct ret code for it.
			 * init result here to avoid affecting the normal path.
			 */
			if (!(link->head->flags & REQ_F_FAIL))
				req_fail_link_node(link->head, -ECANCELED);
		} else if (!(req->flags & (REQ_F_LINK | REQ_F_HARDLINK))) {
			/*
			 * the current req is a normal req, we should return
			 * error and thus break the submittion loop.
			 */
			io_req_complete_failed(req, ret);
			return ret;
		}
		req_fail_link_node(req, ret);
	}

	/* don't need @sqe from now on */
	trace_io_uring_submit_sqe(ctx, req, req->user_data, req->opcode,
				  req->flags, true,
				  ctx->flags & IORING_SETUP_SQPOLL);

	/*
	 * If we already have a head request, queue this one for async
	 * submittal once the head completes. If we don't have a head but
	 * IOSQE_IO_LINK is set in the sqe, start a new head. This one will be
	 * submitted sync once the chain is complete. If none of those
	 * conditions are true (normal request), then just queue it.
	 */
	if (link->head) {
		struct io_kiocb *head = link->head;

		if (!(req->flags & REQ_F_FAIL)) {
			ret = io_req_prep_async(req);
			if (unlikely(ret)) {
				req_fail_link_node(req, ret);
				if (!(head->flags & REQ_F_FAIL))
					req_fail_link_node(head, -ECANCELED);
			}
		}
		trace_io_uring_link(ctx, req, head);
		link->last->link = req;
		link->last = req;

		if (req->flags & (REQ_F_LINK | REQ_F_HARDLINK))
			return 0;
		/* last request of a link, enqueue the link */
		link->head = NULL;
		req = head;
	} else if (req->flags & (REQ_F_LINK | REQ_F_HARDLINK)) {
		link->head = req;
		link->last = req;
		return 0;
	}

	io_queue_sqe(req);
	return 0;