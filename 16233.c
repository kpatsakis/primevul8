static inline unsigned int io_put_kbuf(struct io_kiocb *req,
				       unsigned issue_flags)
{
	unsigned int cflags;

	if (likely(!(req->flags & REQ_F_BUFFER_SELECTED)))
		return 0;

	/*
	 * We can add this buffer back to two lists:
	 *
	 * 1) The io_buffers_cache list. This one is protected by the
	 *    ctx->uring_lock. If we already hold this lock, add back to this
	 *    list as we can grab it from issue as well.
	 * 2) The io_buffers_comp list. This one is protected by the
	 *    ctx->completion_lock.
	 *
	 * We migrate buffers from the comp_list to the issue cache list
	 * when we need one.
	 */
	if (issue_flags & IO_URING_F_UNLOCKED) {
		struct io_ring_ctx *ctx = req->ctx;

		spin_lock(&ctx->completion_lock);
		cflags = __io_put_kbuf(req, &ctx->io_buffers_comp);
		spin_unlock(&ctx->completion_lock);
	} else {
		lockdep_assert_held(&req->ctx->uring_lock);

		cflags = __io_put_kbuf(req, &req->ctx->io_buffers_cache);
	}

	return cflags;
}