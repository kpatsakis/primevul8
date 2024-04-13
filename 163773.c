void http_init_txn(struct session *s)
{
	struct http_txn *txn = &s->txn;
	struct proxy *fe = s->fe;

	txn->flags = 0;
	txn->status = -1;

	global.req_count++;

	txn->cookie_first_date = 0;
	txn->cookie_last_date = 0;

	txn->req.flags = 0;
	txn->req.sol = txn->req.eol = txn->req.eoh = 0; /* relative to the buffer */
	txn->req.next = 0;
	txn->rsp.flags = 0;
	txn->rsp.sol = txn->rsp.eol = txn->rsp.eoh = 0; /* relative to the buffer */
	txn->rsp.next = 0;
	txn->req.chunk_len = 0LL;
	txn->req.body_len = 0LL;
	txn->rsp.chunk_len = 0LL;
	txn->rsp.body_len = 0LL;
	txn->req.msg_state = HTTP_MSG_RQBEFORE; /* at the very beginning of the request */
	txn->rsp.msg_state = HTTP_MSG_RPBEFORE; /* at the very beginning of the response */
	txn->req.chn = s->req;
	txn->rsp.chn = s->rep;

	txn->auth.method = HTTP_AUTH_UNKNOWN;

	txn->req.err_pos = txn->rsp.err_pos = -2; /* block buggy requests/responses */
	if (fe->options2 & PR_O2_REQBUG_OK)
		txn->req.err_pos = -1;            /* let buggy requests pass */

	if (txn->req.cap)
		memset(txn->req.cap, 0, fe->nb_req_cap * sizeof(void *));

	if (txn->rsp.cap)
		memset(txn->rsp.cap, 0, fe->nb_rsp_cap * sizeof(void *));

	if (txn->hdr_idx.v)
		hdr_idx_init(&txn->hdr_idx);
}