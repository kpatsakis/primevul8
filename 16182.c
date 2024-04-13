static bool __io_cqring_overflow_flush(struct io_ring_ctx *ctx, bool force)
{
	bool all_flushed, posted;

	if (!force && __io_cqring_events(ctx) == ctx->cq_entries)
		return false;

	posted = false;
	spin_lock(&ctx->completion_lock);
	while (!list_empty(&ctx->cq_overflow_list)) {
		struct io_uring_cqe *cqe = io_get_cqe(ctx);
		struct io_overflow_cqe *ocqe;

		if (!cqe && !force)
			break;
		ocqe = list_first_entry(&ctx->cq_overflow_list,
					struct io_overflow_cqe, list);
		if (cqe)
			memcpy(cqe, &ocqe->cqe, sizeof(*cqe));
		else
			io_account_cq_overflow(ctx);

		posted = true;
		list_del(&ocqe->list);
		kfree(ocqe);
	}

	all_flushed = list_empty(&ctx->cq_overflow_list);
	if (all_flushed) {
		clear_bit(0, &ctx->check_cq_overflow);
		WRITE_ONCE(ctx->rings->sq_flags,
			   ctx->rings->sq_flags & ~IORING_SQ_CQ_OVERFLOW);
	}

	if (posted)
		io_commit_cqring(ctx);
	spin_unlock(&ctx->completion_lock);
	if (posted)
		io_cqring_ev_posted(ctx);
	return all_flushed;
}