static void ucma_cleanup_mc_events(struct ucma_multicast *mc)
{
	struct ucma_event *uevent, *tmp;

	rdma_lock_handler(mc->ctx->cm_id);
	mutex_lock(&mc->ctx->file->mut);
	list_for_each_entry_safe(uevent, tmp, &mc->ctx->file->event_list, list) {
		if (uevent->mc != mc)
			continue;

		list_del(&uevent->list);
		kfree(uevent);
	}
	mutex_unlock(&mc->ctx->file->mut);
	rdma_unlock_handler(mc->ctx->cm_id);
}