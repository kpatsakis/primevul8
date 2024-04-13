void http_txn_reset_req(struct http_txn *txn)
{
	txn->req.flags = 0;
	txn->req.msg_state = HTTP_MSG_RQBEFORE; /* at the very beginning of the request */
}