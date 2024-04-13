static int ucma_event_handler(struct rdma_cm_id *cm_id,
			      struct rdma_cm_event *event)
{
	struct ucma_event *uevent;
	struct ucma_context *ctx = cm_id->context;

	if (event->event == RDMA_CM_EVENT_CONNECT_REQUEST)
		return ucma_connect_event_handler(cm_id, event);

	/*
	 * We ignore events for new connections until userspace has set their
	 * context.  This can only happen if an error occurs on a new connection
	 * before the user accepts it.  This is okay, since the accept will just
	 * fail later. However, we do need to release the underlying HW
	 * resources in case of a device removal event.
	 */
	if (ctx->uid) {
		uevent = ucma_create_uevent(ctx, event);
		if (!uevent)
			return 0;

		mutex_lock(&ctx->file->mut);
		list_add_tail(&uevent->list, &ctx->file->event_list);
		mutex_unlock(&ctx->file->mut);
		wake_up_interruptible(&ctx->file->poll_wait);
	}

	if (event->event == RDMA_CM_EVENT_DEVICE_REMOVAL && !ctx->destroying)
		queue_work(system_unbound_wq, &ctx->close_work);
	return 0;
}