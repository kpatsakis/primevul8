int stats_check_uri(struct stream_interface *si, struct http_txn *txn, struct proxy *backend)
{
	struct uri_auth *uri_auth = backend->uri_auth;
	struct http_msg *msg = &txn->req;
	const char *uri = msg->chn->buf->p+ msg->sl.rq.u;
	const char *h;

	if (!uri_auth)
		return 0;

	if (txn->meth != HTTP_METH_GET && txn->meth != HTTP_METH_HEAD && txn->meth != HTTP_METH_POST)
		return 0;

	memset(&si->applet.ctx.stats, 0, sizeof(si->applet.ctx.stats));
	si->applet.ctx.stats.st_code = STAT_STATUS_INIT;
	si->applet.ctx.stats.flags |= STAT_FMT_HTML; /* assume HTML mode by default */

	/* check URI size */
	if (uri_auth->uri_len > msg->sl.rq.u_l)
		return 0;

	h = uri;
	if (memcmp(h, uri_auth->uri_prefix, uri_auth->uri_len) != 0)
		return 0;

	h += uri_auth->uri_len;
	while (h <= uri + msg->sl.rq.u_l - 3) {
		if (memcmp(h, ";up", 3) == 0) {
			si->applet.ctx.stats.flags |= STAT_HIDE_DOWN;
			break;
		}
		h++;
	}

	if (uri_auth->refresh) {
		h = uri + uri_auth->uri_len;
		while (h <= uri + msg->sl.rq.u_l - 10) {
			if (memcmp(h, ";norefresh", 10) == 0) {
				si->applet.ctx.stats.flags |= STAT_NO_REFRESH;
				break;
			}
			h++;
		}
	}

	h = uri + uri_auth->uri_len;
	while (h <= uri + msg->sl.rq.u_l - 4) {
		if (memcmp(h, ";csv", 4) == 0) {
			si->applet.ctx.stats.flags &= ~STAT_FMT_HTML;
			break;
		}
		h++;
	}

	h = uri + uri_auth->uri_len;
	while (h <= uri + msg->sl.rq.u_l - 8) {
		if (memcmp(h, ";st=", 4) == 0) {
			int i;
			h += 4;
			si->applet.ctx.stats.st_code = STAT_STATUS_UNKN;
			for (i = STAT_STATUS_INIT + 1; i < STAT_STATUS_SIZE; i++) {
				if (strncmp(stat_status_codes[i], h, 4) == 0) {
					si->applet.ctx.stats.st_code = i;
					break;
				}
			}
			break;
		}
		h++;
	}
	return 1;
}