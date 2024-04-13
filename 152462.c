static unsigned int xenbus_file_poll(struct file *file, poll_table *wait)
{
	struct xenbus_file_priv *u = file->private_data;

	poll_wait(file, &u->read_waitq, wait);
	if (!list_empty(&u->read_buffers))
		return POLLIN | POLLRDNORM;
	return 0;
}