int http_wait_for_request_body(struct stream *s, struct channel *req, int an_bit)
{
	struct session *sess = s->sess;
	struct http_txn *txn = s->txn;
	struct http_msg *msg = &s->txn->req;

	DBG_TRACE_ENTER(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn, msg);


	switch (http_wait_for_msg_body(s, req, s->be->timeout.httpreq, 0)) {
	case HTTP_RULE_RES_CONT:
		goto http_end;
	case HTTP_RULE_RES_YIELD:
		goto missing_data_or_waiting;
	case HTTP_RULE_RES_BADREQ:
		goto return_bad_req;
	case HTTP_RULE_RES_ERROR:
		goto return_int_err;
	case HTTP_RULE_RES_ABRT:
		goto return_prx_cond;
	default:
		goto return_int_err;
	}

 http_end:
	/* The situation will not evolve, so let's give up on the analysis. */
	s->logs.tv_request = now;  /* update the request timer to reflect full request */
	req->analysers &= ~an_bit;
	req->analyse_exp = TICK_ETERNITY;
	DBG_TRACE_LEAVE(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
	return 1;

 missing_data_or_waiting:
	channel_dont_connect(req);
	DBG_TRACE_DEVEL("waiting for more data",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
	return 0;

 return_int_err:
	txn->status = 500;
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_INTERNAL;
	_HA_ATOMIC_INC(&sess->fe->fe_counters.internal_errors);
	if (s->flags & SF_BE_ASSIGNED)
		_HA_ATOMIC_INC(&s->be->be_counters.internal_errors);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->internal_errors);
	goto return_prx_err;

 return_bad_req: /* let's centralize all bad requests */
	txn->status = 400;
	_HA_ATOMIC_INC(&sess->fe->fe_counters.failed_req);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->failed_req);
	/* fall through */

 return_prx_err:
	http_reply_and_close(s, txn->status, http_error_message(s));
	/* fall through */

 return_prx_cond:
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_PRXCOND;
	if (!(s->flags & SF_FINST_MASK))
		s->flags |= (msg->msg_state < HTTP_MSG_DATA ? SF_FINST_R : SF_FINST_D);

	req->analysers &= AN_REQ_FLT_END;
	req->analyse_exp = TICK_ETERNITY;
	DBG_TRACE_DEVEL("leaving on error",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
	return 0;
}