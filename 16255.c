static bool io_cqring_event_overflow(struct io_ring_ctx *ctx, u64 user_data,
				     s32 res, u32 cflags)
{
	struct io_overflow_cqe *ocqe;

	ocqe = kmalloc(sizeof(*ocqe), GFP_ATOMIC | __GFP_ACCOUNT);
	if (!ocqe) {
		/*
		 * If we're in ring overflow flush mode, or in task cancel mode,
		 * or cannot allocate an overflow entry, then we need to drop it
		 * on the floor.
		 */
		io_account_cq_overflow(ctx);
		return false;
	}
	if (list_empty(&ctx->cq_overflow_list)) {
		set_bit(0, &ctx->check_cq_overflow);
		WRITE_ONCE(ctx->rings->sq_flags,
			   ctx->rings->sq_flags | IORING_SQ_CQ_OVERFLOW);

	}
	ocqe->cqe.user_data = user_data;
	ocqe->cqe.res = res;
	ocqe->cqe.flags = cflags;
	list_add_tail(&ocqe->list, &ctx->cq_overflow_list);
	return true;
}