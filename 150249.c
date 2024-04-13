static int ucma_free_ctx(struct ucma_context *ctx)
{
	int events_reported;
	struct ucma_event *uevent, *tmp;
	LIST_HEAD(list);

	ucma_cleanup_multicast(ctx);

	/* Cleanup events not yet reported to the user. */
	mutex_lock(&ctx->file->mut);
	list_for_each_entry_safe(uevent, tmp, &ctx->file->event_list, list) {
		if (uevent->ctx == ctx || uevent->listen_ctx == ctx)
			list_move_tail(&uevent->list, &list);
	}
	list_del(&ctx->list);
	events_reported = ctx->events_reported;
	mutex_unlock(&ctx->file->mut);

	/*
	 * If this was a listening ID then any connections spawned from it
	 * that have not been delivered to userspace are cleaned up too.
	 * Must be done outside any locks.
	 */
	list_for_each_entry_safe(uevent, tmp, &list, list) {
		list_del(&uevent->list);
		if (uevent->resp.event == RDMA_CM_EVENT_CONNECT_REQUEST)
			__destroy_id(uevent->ctx);
		kfree(uevent);
	}

	mutex_destroy(&ctx->mutex);
	kfree(ctx);
	return events_reported;
}