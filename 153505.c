static ssize_t hid_debug_events_read(struct file *file, char __user *buffer,
		size_t count, loff_t *ppos)
{
	struct hid_debug_list *list = file->private_data;
	int ret = 0, copied;
	DECLARE_WAITQUEUE(wait, current);

	mutex_lock(&list->read_mutex);
	if (kfifo_is_empty(&list->hid_debug_fifo)) {
		add_wait_queue(&list->hdev->debug_wait, &wait);
		set_current_state(TASK_INTERRUPTIBLE);

		while (kfifo_is_empty(&list->hid_debug_fifo)) {
			if (file->f_flags & O_NONBLOCK) {
				ret = -EAGAIN;
				break;
			}

			if (signal_pending(current)) {
				ret = -ERESTARTSYS;
				break;
			}

			/* if list->hdev is NULL we cannot remove_wait_queue().
			 * if list->hdev->debug is 0 then hid_debug_unregister()
			 * was already called and list->hdev is being destroyed.
			 * if we add remove_wait_queue() here we can hit a race.
			 */
			if (!list->hdev || !list->hdev->debug) {
				ret = -EIO;
				set_current_state(TASK_RUNNING);
				goto out;
			}

			/* allow O_NONBLOCK from other threads */
			mutex_unlock(&list->read_mutex);
			schedule();
			mutex_lock(&list->read_mutex);
			set_current_state(TASK_INTERRUPTIBLE);
		}

		__set_current_state(TASK_RUNNING);
		remove_wait_queue(&list->hdev->debug_wait, &wait);

		if (ret)
			goto out;
	}

	/* pass the fifo content to userspace, locking is not needed with only
	 * one concurrent reader and one concurrent writer
	 */
	ret = kfifo_to_user(&list->hid_debug_fifo, buffer, count, &copied);
	if (ret)
		goto out;
	ret = copied;
out:
	mutex_unlock(&list->read_mutex);
	return ret;
}