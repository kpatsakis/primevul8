static int xenbus_file_open(struct inode *inode, struct file *filp)
{
	struct xenbus_file_priv *u;

	if (xen_store_evtchn == 0)
		return -ENOENT;

	nonseekable_open(inode, filp);

	u = kzalloc(sizeof(*u), GFP_KERNEL);
	if (u == NULL)
		return -ENOMEM;

	INIT_LIST_HEAD(&u->transactions);
	INIT_LIST_HEAD(&u->watches);
	INIT_LIST_HEAD(&u->read_buffers);
	init_waitqueue_head(&u->read_waitq);

	mutex_init(&u->reply_mutex);
	mutex_init(&u->msgbuffer_mutex);

	filp->private_data = u;

	return 0;
}