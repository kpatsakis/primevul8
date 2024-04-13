
static int io_poll_update(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_kiocb *preq;
	int ret2, ret = 0;
	bool locked;

	spin_lock(&ctx->completion_lock);
	preq = io_poll_find(ctx, req->poll_update.old_user_data, true);
	if (!preq || !io_poll_disarm(preq)) {
		spin_unlock(&ctx->completion_lock);
		ret = preq ? -EALREADY : -ENOENT;
		goto out;
	}
	spin_unlock(&ctx->completion_lock);

	if (req->poll_update.update_events || req->poll_update.update_user_data) {
		/* only mask one event flags, keep behavior flags */
		if (req->poll_update.update_events) {
			preq->poll.events &= ~0xffff;
			preq->poll.events |= req->poll_update.events & 0xffff;
			preq->poll.events |= IO_POLL_UNMASK;
		}
		if (req->poll_update.update_user_data)
			preq->user_data = req->poll_update.new_user_data;

		ret2 = io_poll_add(preq, issue_flags);
		/* successfully updated, don't complete poll request */
		if (!ret2)
			goto out;
	}

	req_set_fail(preq);
	preq->result = -ECANCELED;
	locked = !(issue_flags & IO_URING_F_UNLOCKED);
	io_req_task_complete(preq, &locked);
out:
	if (ret < 0)
		req_set_fail(req);
	/* complete update request, we're done with it */
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;