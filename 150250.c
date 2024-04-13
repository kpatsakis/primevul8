static void ucma_close_id(struct work_struct *work)
{
	struct ucma_context *ctx =  container_of(work, struct ucma_context, close_work);

	/* once all inflight tasks are finished, we close all underlying
	 * resources. The context is still alive till its explicit destryoing
	 * by its creator.
	 */
	ucma_put_ctx(ctx);
	wait_for_completion(&ctx->comp);
	/* No new events will be generated after destroying the id. */
	rdma_destroy_id(ctx->cm_id);

	/*
	 * At this point ctx->ref is zero so the only place the ctx can be is in
	 * a uevent or in __destroy_id(). Since the former doesn't touch
	 * ctx->cm_id and the latter sync cancels this, there is no races with
	 * this store.
	 */
	ctx->cm_id = NULL;
}