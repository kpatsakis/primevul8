void http_perform_server_redirect(struct session *s, struct stream_interface *si)
{
	struct http_txn *txn;
	struct server *srv;
	char *path;
	int len, rewind;

	/* 1: create the response header */
	trash.len = strlen(HTTP_302);
	memcpy(trash.str, HTTP_302, trash.len);

	srv = objt_server(s->target);

	/* 2: add the server's prefix */
	if (trash.len + srv->rdr_len > trash.size)
		return;

	/* special prefix "/" means don't change URL */
	if (srv->rdr_len != 1 || *srv->rdr_pfx != '/') {
		memcpy(trash.str + trash.len, srv->rdr_pfx, srv->rdr_len);
		trash.len += srv->rdr_len;
	}

	/* 3: add the request URI. Since it was already forwarded, we need
	 * to temporarily rewind the buffer.
	 */
	txn = &s->txn;
	b_rew(s->req->buf, rewind = s->req->buf->o);

	path = http_get_path(txn);
	len = buffer_count(s->req->buf, path, b_ptr(s->req->buf, txn->req.sl.rq.u + txn->req.sl.rq.u_l));

	b_adv(s->req->buf, rewind);

	if (!path)
		return;

	if (trash.len + len > trash.size - 4) /* 4 for CRLF-CRLF */
		return;

	memcpy(trash.str + trash.len, path, len);
	trash.len += len;

	if (unlikely(txn->flags & TX_USE_PX_CONN)) {
		memcpy(trash.str + trash.len, "\r\nProxy-Connection: close\r\n\r\n", 29);
		trash.len += 29;
	} else {
		memcpy(trash.str + trash.len, "\r\nConnection: close\r\n\r\n", 23);
		trash.len += 23;
	}

	/* prepare to return without error. */
	si_shutr(si);
	si_shutw(si);
	si->err_type = SI_ET_NONE;
	si->err_loc  = NULL;
	si->state    = SI_ST_CLO;

	/* send the message */
	http_server_error(s, si, SN_ERR_PRXCOND, SN_FINST_C, 302, &trash);

	/* FIXME: we should increase a counter of redirects per server and per backend. */
	srv_inc_sess_ctr(srv);
}