static ssize_t ucma_get_event(struct ucma_file *file, const char __user *inbuf,
			      int in_len, int out_len)
{
	struct rdma_ucm_get_event cmd;
	struct ucma_event *uevent;

	/*
	 * Old 32 bit user space does not send the 4 byte padding in the
	 * reserved field. We don't care, allow it to keep working.
	 */
	if (out_len < sizeof(uevent->resp) - sizeof(uevent->resp.reserved) -
			      sizeof(uevent->resp.ece))
		return -ENOSPC;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	mutex_lock(&file->mut);
	while (list_empty(&file->event_list)) {
		mutex_unlock(&file->mut);

		if (file->filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible(file->poll_wait,
					     !list_empty(&file->event_list)))
			return -ERESTARTSYS;

		mutex_lock(&file->mut);
	}

	uevent = list_first_entry(&file->event_list, struct ucma_event, list);

	if (copy_to_user(u64_to_user_ptr(cmd.response),
			 &uevent->resp,
			 min_t(size_t, out_len, sizeof(uevent->resp)))) {
		mutex_unlock(&file->mut);
		return -EFAULT;
	}

	list_del(&uevent->list);
	uevent->ctx->events_reported++;
	if (uevent->mc)
		uevent->mc->events_reported++;
	if (uevent->resp.event == RDMA_CM_EVENT_CONNECT_REQUEST)
		atomic_inc(&uevent->ctx->backlog);
	mutex_unlock(&file->mut);

	kfree(uevent);
	return 0;
}