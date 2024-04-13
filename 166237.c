void http_destroy_txn(struct stream *s)
{
	struct http_txn *txn = s->txn;

	/* these ones will have been dynamically allocated */
	pool_free(pool_head_requri, txn->uri);
	pool_free(pool_head_capture, txn->cli_cookie);
	pool_free(pool_head_capture, txn->srv_cookie);
	pool_free(pool_head_uniqueid, s->unique_id.ptr);

	s->unique_id = IST_NULL;
	txn->uri = NULL;
	txn->srv_cookie = NULL;
	txn->cli_cookie = NULL;

	if (!LIST_ISEMPTY(&s->vars_txn.head))
		vars_prune(&s->vars_txn, s->sess, s);
	if (!LIST_ISEMPTY(&s->vars_reqres.head))
		vars_prune(&s->vars_reqres, s->sess, s);

	pool_free(pool_head_http_txn, txn);
	s->txn = NULL;
}