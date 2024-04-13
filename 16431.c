static inline bool __io_fill_cqe(struct io_ring_ctx *ctx, u64 user_data,
				 s32 res, u32 cflags)
{
	struct io_uring_cqe *cqe;

	/*
	 * If we can't get a cq entry, userspace overflowed the
	 * submission (by quite a lot). Increment the overflow count in
	 * the ring.
	 */
	cqe = io_get_cqe(ctx);
	if (likely(cqe)) {
		WRITE_ONCE(cqe->user_data, user_data);
		WRITE_ONCE(cqe->res, res);
		WRITE_ONCE(cqe->flags, cflags);
		return true;
	}
	return io_cqring_event_overflow(ctx, user_data, res, cflags);
}