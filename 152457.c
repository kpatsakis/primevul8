static ssize_t xenbus_file_read(struct file *filp,
			       char __user *ubuf,
			       size_t len, loff_t *ppos)
{
	struct xenbus_file_priv *u = filp->private_data;
	struct read_buffer *rb;
	unsigned i;
	int ret;

	mutex_lock(&u->reply_mutex);
again:
	while (list_empty(&u->read_buffers)) {
		mutex_unlock(&u->reply_mutex);
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		ret = wait_event_interruptible(u->read_waitq,
					       !list_empty(&u->read_buffers));
		if (ret)
			return ret;
		mutex_lock(&u->reply_mutex);
	}

	rb = list_entry(u->read_buffers.next, struct read_buffer, list);
	i = 0;
	while (i < len) {
		unsigned sz = min((unsigned)len - i, rb->len - rb->cons);

		ret = copy_to_user(ubuf + i, &rb->msg[rb->cons], sz);

		i += sz - ret;
		rb->cons += sz - ret;

		if (ret != 0) {
			if (i == 0)
				i = -EFAULT;
			goto out;
		}

		/* Clear out buffer if it has been consumed */
		if (rb->cons == rb->len) {
			list_del(&rb->list);
			kfree(rb);
			if (list_empty(&u->read_buffers))
				break;
			rb = list_entry(u->read_buffers.next,
					struct read_buffer, list);
		}
	}
	if (i == 0)
		goto again;

out:
	mutex_unlock(&u->reply_mutex);
	return i;
}