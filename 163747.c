int http_resync_states(struct session *s)
{
	struct http_txn *txn = &s->txn;
	int old_req_state = txn->req.msg_state;
	int old_res_state = txn->rsp.msg_state;

	http_silent_debug(__LINE__, s);
	http_sync_req_state(s);
	while (1) {
		http_silent_debug(__LINE__, s);
		if (!http_sync_res_state(s))
			break;
		http_silent_debug(__LINE__, s);
		if (!http_sync_req_state(s))
			break;
	}
	http_silent_debug(__LINE__, s);
	/* OK, both state machines agree on a compatible state.
	 * There are a few cases we're interested in :
	 *  - HTTP_MSG_TUNNEL on either means we have to disable both analysers
	 *  - HTTP_MSG_CLOSED on both sides means we've reached the end in both
	 *    directions, so let's simply disable both analysers.
	 *  - HTTP_MSG_CLOSED on the response only means we must abort the
	 *    request.
	 *  - HTTP_MSG_CLOSED on the request and HTTP_MSG_DONE on the response
	 *    with server-close mode means we've completed one request and we
	 *    must re-initialize the server connection.
	 */

	if (txn->req.msg_state == HTTP_MSG_TUNNEL ||
	    txn->rsp.msg_state == HTTP_MSG_TUNNEL ||
	    (txn->req.msg_state == HTTP_MSG_CLOSED &&
	     txn->rsp.msg_state == HTTP_MSG_CLOSED)) {
		s->req->analysers = 0;
		channel_auto_close(s->req);
		channel_auto_read(s->req);
		s->rep->analysers = 0;
		channel_auto_close(s->rep);
		channel_auto_read(s->rep);
	}
	else if ((txn->req.msg_state >= HTTP_MSG_DONE &&
		  (txn->rsp.msg_state == HTTP_MSG_CLOSED || (s->rep->flags & CF_SHUTW))) ||
		 txn->rsp.msg_state == HTTP_MSG_ERROR ||
		 txn->req.msg_state == HTTP_MSG_ERROR) {
		s->rep->analysers = 0;
		channel_auto_close(s->rep);
		channel_auto_read(s->rep);
		s->req->analysers = 0;
		channel_abort(s->req);
		channel_auto_close(s->req);
		channel_auto_read(s->req);
		bi_erase(s->req);
	}
	else if (txn->req.msg_state == HTTP_MSG_CLOSED &&
		 txn->rsp.msg_state == HTTP_MSG_DONE &&
		 ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL)) {
		/* server-close: terminate this server connection and
		 * reinitialize a fresh-new transaction.
		 */
		http_end_txn_clean_session(s);
	}

	http_silent_debug(__LINE__, s);
	return txn->req.msg_state != old_req_state ||
		txn->rsp.msg_state != old_res_state;
}