static int __destroy_id(struct ucma_context *ctx)
{
	/*
	 * If the refcount is already 0 then ucma_close_id() has already
	 * destroyed the cm_id, otherwise holding the refcount keeps cm_id
	 * valid. Prevent queue_work() from being called.
	 */
	if (refcount_inc_not_zero(&ctx->ref)) {
		rdma_lock_handler(ctx->cm_id);
		ctx->destroying = 1;
		rdma_unlock_handler(ctx->cm_id);
		ucma_put_ctx(ctx);
	}

	cancel_work_sync(&ctx->close_work);
	/* At this point it's guaranteed that there is no inflight closing task */
	if (ctx->cm_id)
		ucma_close_id(&ctx->close_work);
	return ucma_free_ctx(ctx);
}