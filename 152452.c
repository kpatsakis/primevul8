static void watch_fired(struct xenbus_watch *watch,
			const char **vec,
			unsigned int len)
{
	struct watch_adapter *adap;
	struct xsd_sockmsg hdr;
	const char *path, *token;
	int path_len, tok_len, body_len, data_len = 0;
	int ret;
	LIST_HEAD(staging_q);

	adap = container_of(watch, struct watch_adapter, watch);

	path = vec[XS_WATCH_PATH];
	token = adap->token;

	path_len = strlen(path) + 1;
	tok_len = strlen(token) + 1;
	if (len > 2)
		data_len = vec[len] - vec[2] + 1;
	body_len = path_len + tok_len + data_len;

	hdr.type = XS_WATCH_EVENT;
	hdr.len = body_len;

	mutex_lock(&adap->dev_data->reply_mutex);

	ret = queue_reply(&staging_q, &hdr, sizeof(hdr));
	if (!ret)
		ret = queue_reply(&staging_q, path, path_len);
	if (!ret)
		ret = queue_reply(&staging_q, token, tok_len);
	if (!ret && len > 2)
		ret = queue_reply(&staging_q, vec[2], data_len);

	if (!ret) {
		/* success: pass reply list onto watcher */
		list_splice_tail(&staging_q, &adap->dev_data->read_buffers);
		wake_up(&adap->dev_data->read_waitq);
	} else
		queue_cleanup(&staging_q);

	mutex_unlock(&adap->dev_data->reply_mutex);
}