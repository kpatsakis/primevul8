static __poll_t ucma_poll(struct file *filp, struct poll_table_struct *wait)
{
	struct ucma_file *file = filp->private_data;
	__poll_t mask = 0;

	poll_wait(filp, &file->poll_wait, wait);

	if (!list_empty(&file->event_list))
		mask = EPOLLIN | EPOLLRDNORM;

	return mask;
}