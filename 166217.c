enum rule_result http_wait_for_msg_body(struct stream *s, struct channel *chn,
					unsigned int time, unsigned int bytes)
{
	struct session *sess = s->sess;
	struct http_txn *txn = s->txn;
	struct http_msg *msg = ((chn->flags & CF_ISRESP) ? &txn->rsp : &txn->req);
	struct htx *htx;
	enum rule_result ret = HTTP_RULE_RES_CONT;

	htx = htxbuf(&chn->buf);

	if (htx->flags & HTX_FL_PARSING_ERROR) {
		ret = HTTP_RULE_RES_BADREQ;
		goto end;
	}
	if (htx->flags & HTX_FL_PROCESSING_ERROR) {
		ret = HTTP_RULE_RES_ERROR;
		goto end;
	}

	/* Do nothing for bodyless and CONNECT requests */
	if (txn->meth == HTTP_METH_CONNECT || (msg->flags & HTTP_MSGF_BODYLESS))
		goto end;

	if (!(chn->flags & CF_ISRESP) && msg->msg_state < HTTP_MSG_DATA) {
		if (http_handle_expect_hdr(s, htx, msg) == -1) {
			ret = HTTP_RULE_RES_ERROR;
			goto end;
		}
	}

	msg->msg_state = HTTP_MSG_DATA;

	/* Now we're in HTTP_MSG_DATA. We just need to know if all data have
	 * been received or if the buffer is full.
	 */
	if ((htx->flags & HTX_FL_EOM) ||
	    htx_get_tail_type(htx) > HTX_BLK_DATA ||
	    channel_htx_full(chn, htx, global.tune.maxrewrite) ||
	    si_rx_blocked_room(chn_prod(chn)))
		goto end;

	if (bytes) {
		struct htx_blk *blk;
		unsigned int len = 0;

		for (blk = htx_get_first_blk(htx); blk; blk = htx_get_next_blk(htx, blk)) {
			if (htx_get_blk_type(blk) != HTX_BLK_DATA)
				continue;
			len += htx_get_blksz(blk);
			if (len >= bytes)
				goto end;
		}
	}

	if ((chn->flags & CF_READ_TIMEOUT) || tick_is_expired(chn->analyse_exp, now_ms)) {
		if (!(chn->flags & CF_ISRESP))
			goto abort_req;
		goto abort_res;
	}

	/* we get here if we need to wait for more data */
	if (!(chn->flags & (CF_SHUTR | CF_READ_ERROR))) {
		if (!tick_isset(chn->analyse_exp))
			chn->analyse_exp = tick_add_ifset(now_ms, time);
		ret = HTTP_RULE_RES_YIELD;
	}

  end:
	return ret;

  abort_req:
	txn->status = 408;
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_CLITO;
	if (!(s->flags & SF_FINST_MASK))
		s->flags |= SF_FINST_D;
	_HA_ATOMIC_INC(&sess->fe->fe_counters.failed_req);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->failed_req);
	http_reply_and_close(s, txn->status, http_error_message(s));
	ret = HTTP_RULE_RES_ABRT;
	goto end;

  abort_res:
	txn->status = 504;
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_SRVTO;
	if (!(s->flags & SF_FINST_MASK))
		s->flags |= SF_FINST_D;
	stream_inc_http_fail_ctr(s);
	http_reply_and_close(s, txn->status, http_error_message(s));
	ret = HTTP_RULE_RES_ABRT;
	goto end;
}