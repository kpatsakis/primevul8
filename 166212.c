void http_txn_reset_res(struct http_txn *txn)
{
	txn->rsp.flags = 0;
	txn->rsp.msg_state = HTTP_MSG_RPBEFORE; /* at the very beginning of the response */
}