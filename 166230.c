static int http_stats_check_uri(struct stream *s, struct http_txn *txn, struct proxy *backend)
{
	struct uri_auth *uri_auth = backend->uri_auth;
	struct htx *htx;
	struct htx_sl *sl;
	struct ist uri;

	if (!uri_auth)
		return 0;

	if (txn->meth != HTTP_METH_GET && txn->meth != HTTP_METH_HEAD && txn->meth != HTTP_METH_POST)
		return 0;

	htx = htxbuf(&s->req.buf);
	sl = http_get_stline(htx);
	uri = htx_sl_req_uri(sl);
	if (*uri_auth->uri_prefix == '/') {
		struct http_uri_parser parser = http_uri_parser_init(uri);
		uri = http_parse_path(&parser);
	}

	/* check URI size */
	if (uri_auth->uri_len > uri.len)
		return 0;

	if (memcmp(uri.ptr, uri_auth->uri_prefix, uri_auth->uri_len) != 0)
		return 0;

	return 1;
}