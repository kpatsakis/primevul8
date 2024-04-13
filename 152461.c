static int xenbus_write_watch(unsigned msg_type, struct xenbus_file_priv *u)
{
	struct watch_adapter *watch, *tmp_watch;
	char *path, *token;
	int err, rc;
	LIST_HEAD(staging_q);

	path = u->u.buffer + sizeof(u->u.msg);
	token = memchr(path, 0, u->u.msg.len);
	if (token == NULL) {
		rc = -EILSEQ;
		goto out;
	}
	token++;
	if (memchr(token, 0, u->u.msg.len - (token - path)) == NULL) {
		rc = -EILSEQ;
		goto out;
	}

	if (msg_type == XS_WATCH) {
		watch = alloc_watch_adapter(path, token);
		if (watch == NULL) {
			rc = -ENOMEM;
			goto out;
		}

		watch->watch.callback = watch_fired;
		watch->dev_data = u;

		err = register_xenbus_watch(&watch->watch);
		if (err) {
			free_watch_adapter(watch);
			rc = err;
			goto out;
		}
		list_add(&watch->list, &u->watches);
	} else {
		list_for_each_entry_safe(watch, tmp_watch, &u->watches, list) {
			if (!strcmp(watch->token, token) &&
			    !strcmp(watch->watch.node, path)) {
				unregister_xenbus_watch(&watch->watch);
				list_del(&watch->list);
				free_watch_adapter(watch);
				break;
			}
		}
	}

	/* Success.  Synthesize a reply to say all is OK. */
	{
		struct {
			struct xsd_sockmsg hdr;
			char body[3];
		} __packed reply = {
			{
				.type = msg_type,
				.len = sizeof(reply.body)
			},
			"OK"
		};

		mutex_lock(&u->reply_mutex);
		rc = queue_reply(&u->read_buffers, &reply, sizeof(reply));
		wake_up(&u->read_waitq);
		mutex_unlock(&u->reply_mutex);
	}

out:
	return rc;
}