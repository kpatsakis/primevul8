static int ucma_connect_event_handler(struct rdma_cm_id *cm_id,
				      struct rdma_cm_event *event)
{
	struct ucma_context *listen_ctx = cm_id->context;
	struct ucma_context *ctx;
	struct ucma_event *uevent;

	if (!atomic_add_unless(&listen_ctx->backlog, -1, 0))
		return -ENOMEM;
	ctx = ucma_alloc_ctx(listen_ctx->file);
	if (!ctx)
		goto err_backlog;
	ctx->cm_id = cm_id;

	uevent = ucma_create_uevent(listen_ctx, event);
	if (!uevent)
		goto err_alloc;
	uevent->listen_ctx = listen_ctx;
	uevent->resp.id = ctx->id;

	ctx->cm_id->context = ctx;

	mutex_lock(&ctx->file->mut);
	ucma_finish_ctx(ctx);
	list_add_tail(&uevent->list, &ctx->file->event_list);
	mutex_unlock(&ctx->file->mut);
	wake_up_interruptible(&ctx->file->poll_wait);
	return 0;

err_alloc:
	xa_erase(&ctx_table, ctx->id);
	kfree(ctx);
err_backlog:
	atomic_inc(&listen_ctx->backlog);
	/* Returning error causes the new ID to be destroyed */
	return -ENOMEM;
}