static inline void io_req_set_rsrc_node(struct io_kiocb *req,
					struct io_ring_ctx *ctx,
					unsigned int issue_flags)
{
	if (!req->fixed_rsrc_refs) {
		req->fixed_rsrc_refs = &ctx->rsrc_node->refs;

		if (!(issue_flags & IO_URING_F_UNLOCKED)) {
			lockdep_assert_held(&ctx->uring_lock);
			ctx->rsrc_cached_refs--;
			if (unlikely(ctx->rsrc_cached_refs < 0))
				io_rsrc_refs_refill(ctx);
		} else {
			percpu_ref_get(req->fixed_rsrc_refs);
		}
	}
}