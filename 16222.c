
static int io_timeout(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_timeout_data *data = req->async_data;
	struct list_head *entry;
	u32 tail, off = req->timeout.off;

	spin_lock_irq(&ctx->timeout_lock);

	/*
	 * sqe->off holds how many events that need to occur for this
	 * timeout event to be satisfied. If it isn't set, then this is
	 * a pure timeout request, sequence isn't used.
	 */
	if (io_is_timeout_noseq(req)) {
		entry = ctx->timeout_list.prev;
		goto add;
	}

	tail = ctx->cached_cq_tail - atomic_read(&ctx->cq_timeouts);
	req->timeout.target_seq = tail + off;

	/* Update the last seq here in case io_flush_timeouts() hasn't.
	 * This is safe because ->completion_lock is held, and submissions
	 * and completions are never mixed in the same ->completion_lock section.
	 */
	ctx->cq_last_tm_flush = tail;

	/*
	 * Insertion sort, ensuring the first entry in the list is always
	 * the one we need first.
	 */
	list_for_each_prev(entry, &ctx->timeout_list) {
		struct io_kiocb *nxt = list_entry(entry, struct io_kiocb,
						  timeout.list);

		if (io_is_timeout_noseq(nxt))
			continue;
		/* nxt.seq is behind @tail, otherwise would've been completed */
		if (off >= nxt->timeout.target_seq - tail)
			break;
	}
add:
	list_add(&req->timeout.list, entry);
	data->timer.function = io_timeout_fn;
	hrtimer_start(&data->timer, timespec64_to_ktime(data->ts), data->mode);
	spin_unlock_irq(&ctx->timeout_lock);
	return 0;