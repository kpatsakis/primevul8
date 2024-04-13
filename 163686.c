void http_end_txn(struct session *s)
{
	struct http_txn *txn = &s->txn;

	/* these ones will have been dynamically allocated */
	pool_free2(pool2_requri, txn->uri);
	pool_free2(pool2_capture, txn->cli_cookie);
	pool_free2(pool2_capture, txn->srv_cookie);
	pool_free2(apools.sessid, txn->sessid);
	pool_free2(pool2_uniqueid, s->unique_id);

	s->unique_id = NULL;
	txn->sessid = NULL;
	txn->uri = NULL;
	txn->srv_cookie = NULL;
	txn->cli_cookie = NULL;

	if (txn->req.cap) {
		struct cap_hdr *h;
		for (h = s->fe->req_cap; h; h = h->next)
			pool_free2(h->pool, txn->req.cap[h->index]);
		memset(txn->req.cap, 0, s->fe->nb_req_cap * sizeof(void *));
	}

	if (txn->rsp.cap) {
		struct cap_hdr *h;
		for (h = s->fe->rsp_cap; h; h = h->next)
			pool_free2(h->pool, txn->rsp.cap[h->index]);
		memset(txn->rsp.cap, 0, s->fe->nb_rsp_cap * sizeof(void *));
	}

}