
static void io_req_caches_free(struct io_ring_ctx *ctx)
{
	struct io_submit_state *state = &ctx->submit_state;
	int nr = 0;

	mutex_lock(&ctx->uring_lock);
	io_flush_cached_locked_reqs(ctx, state);

	while (state->free_list.next) {
		struct io_wq_work_node *node;
		struct io_kiocb *req;

		node = wq_stack_extract(&state->free_list);
		req = container_of(node, struct io_kiocb, comp_list);
		kmem_cache_free(req_cachep, req);
		nr++;
	}
	if (nr)
		percpu_ref_put_many(&ctx->refs, nr);
	mutex_unlock(&ctx->uring_lock);