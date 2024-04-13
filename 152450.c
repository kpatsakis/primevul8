static ssize_t xenbus_file_write(struct file *filp,
				const char __user *ubuf,
				size_t len, loff_t *ppos)
{
	struct xenbus_file_priv *u = filp->private_data;
	uint32_t msg_type;
	int rc = len;
	int ret;
	LIST_HEAD(staging_q);

	/*
	 * We're expecting usermode to be writing properly formed
	 * xenbus messages.  If they write an incomplete message we
	 * buffer it up.  Once it is complete, we act on it.
	 */

	/*
	 * Make sure concurrent writers can't stomp all over each
	 * other's messages and make a mess of our partial message
	 * buffer.  We don't make any attemppt to stop multiple
	 * writers from making a mess of each other's incomplete
	 * messages; we're just trying to guarantee our own internal
	 * consistency and make sure that single writes are handled
	 * atomically.
	 */
	mutex_lock(&u->msgbuffer_mutex);

	/* Get this out of the way early to avoid confusion */
	if (len == 0)
		goto out;

	/* Can't write a xenbus message larger we can buffer */
	if (len > sizeof(u->u.buffer) - u->len) {
		/* On error, dump existing buffer */
		u->len = 0;
		rc = -EINVAL;
		goto out;
	}

	ret = copy_from_user(u->u.buffer + u->len, ubuf, len);

	if (ret != 0) {
		rc = -EFAULT;
		goto out;
	}

	/* Deal with a partial copy. */
	len -= ret;
	rc = len;

	u->len += len;

	/* Return if we haven't got a full message yet */
	if (u->len < sizeof(u->u.msg))
		goto out;	/* not even the header yet */

	/* If we're expecting a message that's larger than we can
	   possibly send, dump what we have and return an error. */
	if ((sizeof(u->u.msg) + u->u.msg.len) > sizeof(u->u.buffer)) {
		rc = -E2BIG;
		u->len = 0;
		goto out;
	}

	if (u->len < (sizeof(u->u.msg) + u->u.msg.len))
		goto out;	/* incomplete data portion */

	/*
	 * OK, now we have a complete message.  Do something with it.
	 */

	msg_type = u->u.msg.type;

	switch (msg_type) {
	case XS_WATCH:
	case XS_UNWATCH:
		/* (Un)Ask for some path to be watched for changes */
		ret = xenbus_write_watch(msg_type, u);
		break;

	default:
		/* Send out a transaction */
		ret = xenbus_write_transaction(msg_type, u);
		break;
	}
	if (ret != 0)
		rc = ret;

	/* Buffered message consumed */
	u->len = 0;

 out:
	mutex_unlock(&u->msgbuffer_mutex);
	return rc;
}