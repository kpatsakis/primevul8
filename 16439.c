
static enum hrtimer_restart io_link_timeout_fn(struct hrtimer *timer)
{
	struct io_timeout_data *data = container_of(timer,
						struct io_timeout_data, timer);
	struct io_kiocb *prev, *req = data->req;
	struct io_ring_ctx *ctx = req->ctx;
	unsigned long flags;

	spin_lock_irqsave(&ctx->timeout_lock, flags);
	prev = req->timeout.head;
	req->timeout.head = NULL;

	/*
	 * We don't expect the list to be empty, that will only happen if we
	 * race with the completion of the linked work.
	 */
	if (prev) {
		io_remove_next_linked(prev);
		if (!req_ref_inc_not_zero(prev))
			prev = NULL;
	}
	list_del(&req->timeout.list);
	req->timeout.prev = prev;
	spin_unlock_irqrestore(&ctx->timeout_lock, flags);

	req->io_task_work.func = io_req_task_link_timeout;
	io_req_task_work_add(req, false);
	return HRTIMER_NORESTART;