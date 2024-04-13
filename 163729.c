get_http_auth(struct session *s)
{

	struct http_txn *txn = &s->txn;
	struct chunk auth_method;
	struct hdr_ctx ctx;
	char *h, *p;
	int len;

#ifdef DEBUG_AUTH
	printf("Auth for session %p: %d\n", s, txn->auth.method);
#endif

	if (txn->auth.method == HTTP_AUTH_WRONG)
		return 0;

	if (txn->auth.method)
		return 1;

	txn->auth.method = HTTP_AUTH_WRONG;

	ctx.idx = 0;

	if (txn->flags & TX_USE_PX_CONN) {
		h = "Proxy-Authorization";
		len = strlen(h);
	} else {
		h = "Authorization";
		len = strlen(h);
	}

	if (!http_find_header2(h, len, s->req->buf->p, &txn->hdr_idx, &ctx))
		return 0;

	h = ctx.line + ctx.val;

	p = memchr(h, ' ', ctx.vlen);
	if (!p || p == h)
		return 0;

	chunk_initlen(&auth_method, h, 0, p-h);
	chunk_initlen(&txn->auth.method_data, p+1, 0, ctx.vlen-(p-h)-1);

	if (!strncasecmp("Basic", auth_method.str, auth_method.len)) {

		len = base64dec(txn->auth.method_data.str, txn->auth.method_data.len,
				get_http_auth_buff, global.tune.bufsize - 1);

		if (len < 0)
			return 0;


		get_http_auth_buff[len] = '\0';

		p = strchr(get_http_auth_buff, ':');

		if (!p)
			return 0;

		txn->auth.user = get_http_auth_buff;
		*p = '\0';
		txn->auth.pass = p+1;

		txn->auth.method = HTTP_AUTH_BASIC;
		return 1;
	}

	return 0;
}