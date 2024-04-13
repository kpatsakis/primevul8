static __cold struct io_ring_ctx *io_ring_ctx_alloc(struct io_uring_params *p)
{
	struct io_ring_ctx *ctx;
	int i, hash_bits;

	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return NULL;

	/*
	 * Use 5 bits less than the max cq entries, that should give us around
	 * 32 entries per hash list if totally full and uniformly spread.
	 */
	hash_bits = ilog2(p->cq_entries);
	hash_bits -= 5;
	if (hash_bits <= 0)
		hash_bits = 1;
	ctx->cancel_hash_bits = hash_bits;
	ctx->cancel_hash = kmalloc((1U << hash_bits) * sizeof(struct hlist_head),
					GFP_KERNEL);
	if (!ctx->cancel_hash)
		goto err;
	__hash_init(ctx->cancel_hash, 1U << hash_bits);

	ctx->dummy_ubuf = kzalloc(sizeof(*ctx->dummy_ubuf), GFP_KERNEL);
	if (!ctx->dummy_ubuf)
		goto err;
	/* set invalid range, so io_import_fixed() fails meeting it */
	ctx->dummy_ubuf->ubuf = -1UL;

	ctx->io_buffers = kcalloc(1U << IO_BUFFERS_HASH_BITS,
					sizeof(struct list_head), GFP_KERNEL);
	if (!ctx->io_buffers)
		goto err;
	for (i = 0; i < (1U << IO_BUFFERS_HASH_BITS); i++)
		INIT_LIST_HEAD(&ctx->io_buffers[i]);

	if (percpu_ref_init(&ctx->refs, io_ring_ctx_ref_free,
			    PERCPU_REF_ALLOW_REINIT, GFP_KERNEL))
		goto err;

	ctx->flags = p->flags;
	init_waitqueue_head(&ctx->sqo_sq_wait);
	INIT_LIST_HEAD(&ctx->sqd_list);
	INIT_LIST_HEAD(&ctx->cq_overflow_list);
	INIT_LIST_HEAD(&ctx->io_buffers_cache);
	INIT_LIST_HEAD(&ctx->apoll_cache);
	init_completion(&ctx->ref_comp);
	xa_init_flags(&ctx->personalities, XA_FLAGS_ALLOC1);
	mutex_init(&ctx->uring_lock);
	init_waitqueue_head(&ctx->cq_wait);
	spin_lock_init(&ctx->completion_lock);
	spin_lock_init(&ctx->timeout_lock);
	INIT_WQ_LIST(&ctx->iopoll_list);
	INIT_LIST_HEAD(&ctx->io_buffers_pages);
	INIT_LIST_HEAD(&ctx->io_buffers_comp);
	INIT_LIST_HEAD(&ctx->defer_list);
	INIT_LIST_HEAD(&ctx->timeout_list);
	INIT_LIST_HEAD(&ctx->ltimeout_list);
	spin_lock_init(&ctx->rsrc_ref_lock);
	INIT_LIST_HEAD(&ctx->rsrc_ref_list);
	INIT_DELAYED_WORK(&ctx->rsrc_put_work, io_rsrc_put_work);
	init_llist_head(&ctx->rsrc_put_llist);
	INIT_LIST_HEAD(&ctx->tctx_list);
	ctx->submit_state.free_list.next = NULL;
	INIT_WQ_LIST(&ctx->locked_free_list);
	INIT_DELAYED_WORK(&ctx->fallback_work, io_fallback_req_func);
	INIT_WQ_LIST(&ctx->submit_state.compl_reqs);
	return ctx;
err:
	kfree(ctx->dummy_ubuf);
	kfree(ctx->cancel_hash);
	kfree(ctx->io_buffers);
	kfree(ctx);
	return NULL;
}