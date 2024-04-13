struct http_txn *http_create_txn(struct stream *s)
{
	struct http_txn *txn;
	struct conn_stream *cs = objt_cs(s->si[0].end);

	txn = pool_alloc(pool_head_http_txn);
	if (!txn)
		return NULL;
	s->txn = txn;

	txn->flags = ((cs && cs->flags & CS_FL_NOT_FIRST) ? TX_NOT_FIRST : 0);
	txn->status = -1;
	txn->http_reply = NULL;
	write_u32(txn->cache_hash, 0);

	txn->cookie_first_date = 0;
	txn->cookie_last_date = 0;

	txn->srv_cookie = NULL;
	txn->cli_cookie = NULL;
	txn->uri = NULL;

	http_txn_reset_req(txn);
	http_txn_reset_res(txn);

	txn->req.chn = &s->req;
	txn->rsp.chn = &s->res;

	txn->auth.method = HTTP_AUTH_UNKNOWN;

	vars_init_head(&s->vars_txn,    SCOPE_TXN);
	vars_init_head(&s->vars_reqres, SCOPE_REQ);

	return txn;
}