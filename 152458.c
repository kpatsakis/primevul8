static int xenbus_file_release(struct inode *inode, struct file *filp)
{
	struct xenbus_file_priv *u = filp->private_data;
	struct xenbus_transaction_holder *trans, *tmp;
	struct watch_adapter *watch, *tmp_watch;
	struct read_buffer *rb, *tmp_rb;

	/*
	 * No need for locking here because there are no other users,
	 * by definition.
	 */

	list_for_each_entry_safe(trans, tmp, &u->transactions, list) {
		xenbus_transaction_end(trans->handle, 1);
		list_del(&trans->list);
		kfree(trans);
	}

	list_for_each_entry_safe(watch, tmp_watch, &u->watches, list) {
		unregister_xenbus_watch(&watch->watch);
		list_del(&watch->list);
		free_watch_adapter(watch);
	}

	list_for_each_entry_safe(rb, tmp_rb, &u->read_buffers, list) {
		list_del(&rb->list);
		kfree(rb);
	}
	kfree(u);

	return 0;
}