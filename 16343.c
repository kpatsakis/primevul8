
static enum hrtimer_restart io_timeout_fn(struct hrtimer *timer)
{
	struct io_timeout_data *data = container_of(timer,
						struct io_timeout_data, timer);
	struct io_kiocb *req = data->req;
	struct io_ring_ctx *ctx = req->ctx;
	unsigned long flags;

	spin_lock_irqsave(&ctx->timeout_lock, flags);
	list_del_init(&req->timeout.list);
	atomic_set(&req->ctx->cq_timeouts,
		atomic_read(&req->ctx->cq_timeouts) + 1);
	spin_unlock_irqrestore(&ctx->timeout_lock, flags);

	if (!(data->flags & IORING_TIMEOUT_ETIME_SUCCESS))
		req_set_fail(req);

	req->result = -ETIME;
	req->io_task_work.func = io_req_task_complete;
	io_req_task_work_add(req, false);
	return HRTIMER_NORESTART;