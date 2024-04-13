	__must_hold(&ctx->uring_lock)
{
	struct io_submit_state *state = &ctx->submit_state;
	gfp_t gfp = GFP_KERNEL | __GFP_NOWARN;
	void *reqs[IO_REQ_ALLOC_BATCH];
	struct io_kiocb *req;
	int ret, i;

	if (likely(state->free_list.next || io_flush_cached_reqs(ctx)))
		return true;

	ret = kmem_cache_alloc_bulk(req_cachep, gfp, ARRAY_SIZE(reqs), reqs);

	/*
	 * Bulk alloc is all-or-nothing. If we fail to get a batch,
	 * retry single alloc to be on the safe side.
	 */
	if (unlikely(ret <= 0)) {
		reqs[0] = kmem_cache_alloc(req_cachep, gfp);
		if (!reqs[0])
			return false;
		ret = 1;
	}

	percpu_ref_get_many(&ctx->refs, ret);
	for (i = 0; i < ret; i++) {
		req = reqs[i];

		io_preinit_req(req, ctx);
		wq_stack_add_head(&req->comp_list, &state->free_list);
	}
	return true;
}