static int http_apply_redirect_rule(struct redirect_rule *rule, struct session *s, struct http_txn *txn)
{
	struct http_msg *msg = &txn->req;
	const char *msg_fmt;

	/* build redirect message */
	switch(rule->code) {
	case 308:
		msg_fmt = HTTP_308;
		break;
	case 307:
		msg_fmt = HTTP_307;
		break;
	case 303:
		msg_fmt = HTTP_303;
		break;
	case 301:
		msg_fmt = HTTP_301;
		break;
	case 302:
	default:
		msg_fmt = HTTP_302;
		break;
	}

	if (unlikely(!chunk_strcpy(&trash, msg_fmt)))
		return 0;

	switch(rule->type) {
	case REDIRECT_TYPE_SCHEME: {
		const char *path;
		const char *host;
		struct hdr_ctx ctx;
		int pathlen;
		int hostlen;

		host = "";
		hostlen = 0;
		ctx.idx = 0;
		if (http_find_header2("Host", 4, txn->req.chn->buf->p + txn->req.sol, &txn->hdr_idx, &ctx)) {
			host = ctx.line + ctx.val;
			hostlen = ctx.vlen;
		}

		path = http_get_path(txn);
		/* build message using path */
		if (path) {
			pathlen = txn->req.sl.rq.u_l + (txn->req.chn->buf->p + txn->req.sl.rq.u) - path;
			if (rule->flags & REDIRECT_FLAG_DROP_QS) {
				int qs = 0;
				while (qs < pathlen) {
					if (path[qs] == '?') {
						pathlen = qs;
						break;
					}
					qs++;
				}
			}
		} else {
			path = "/";
			pathlen = 1;
		}

		/* check if we can add scheme + "://" + host + path */
		if (trash.len + rule->rdr_len + 3 + hostlen + pathlen > trash.size - 4)
			return 0;

		/* add scheme */
		memcpy(trash.str + trash.len, rule->rdr_str, rule->rdr_len);
		trash.len += rule->rdr_len;

		/* add "://" */
		memcpy(trash.str + trash.len, "://", 3);
		trash.len += 3;

		/* add host */
		memcpy(trash.str + trash.len, host, hostlen);
		trash.len += hostlen;

		/* add path */
		memcpy(trash.str + trash.len, path, pathlen);
		trash.len += pathlen;

		/* append a slash at the end of the location is needed and missing */
		if (trash.len && trash.str[trash.len - 1] != '/' &&
		    (rule->flags & REDIRECT_FLAG_APPEND_SLASH)) {
			if (trash.len > trash.size - 5)
				return 0;
			trash.str[trash.len] = '/';
			trash.len++;
		}

		break;
	}
	case REDIRECT_TYPE_PREFIX: {
		const char *path;
		int pathlen;

		path = http_get_path(txn);
		/* build message using path */
		if (path) {
			pathlen = txn->req.sl.rq.u_l + (txn->req.chn->buf->p + txn->req.sl.rq.u) - path;
			if (rule->flags & REDIRECT_FLAG_DROP_QS) {
				int qs = 0;
				while (qs < pathlen) {
					if (path[qs] == '?') {
						pathlen = qs;
						break;
					}
					qs++;
				}
			}
		} else {
			path = "/";
			pathlen = 1;
		}

		if (trash.len + rule->rdr_len + pathlen > trash.size - 4)
			return 0;

		/* add prefix. Note that if prefix == "/", we don't want to
		 * add anything, otherwise it makes it hard for the user to
		 * configure a self-redirection.
		 */
		if (rule->rdr_len != 1 || *rule->rdr_str != '/') {
			memcpy(trash.str + trash.len, rule->rdr_str, rule->rdr_len);
			trash.len += rule->rdr_len;
		}

		/* add path */
		memcpy(trash.str + trash.len, path, pathlen);
		trash.len += pathlen;

		/* append a slash at the end of the location is needed and missing */
		if (trash.len && trash.str[trash.len - 1] != '/' &&
		    (rule->flags & REDIRECT_FLAG_APPEND_SLASH)) {
			if (trash.len > trash.size - 5)
				return 0;
			trash.str[trash.len] = '/';
			trash.len++;
		}

		break;
	}
	case REDIRECT_TYPE_LOCATION:
	default:
		if (trash.len + rule->rdr_len > trash.size - 4)
			return 0;

		/* add location */
		memcpy(trash.str + trash.len, rule->rdr_str, rule->rdr_len);
		trash.len += rule->rdr_len;
		break;
	}

	if (rule->cookie_len) {
		memcpy(trash.str + trash.len, "\r\nSet-Cookie: ", 14);
		trash.len += 14;
		memcpy(trash.str + trash.len, rule->cookie_str, rule->cookie_len);
		trash.len += rule->cookie_len;
		memcpy(trash.str + trash.len, "\r\n", 2);
		trash.len += 2;
	}

	/* add end of headers and the keep-alive/close status.
	 * We may choose to set keep-alive if the Location begins
	 * with a slash, because the client will come back to the
	 * same server.
	 */
	txn->status = rule->code;
	/* let's log the request time */
	s->logs.tv_request = now;

	if (rule->rdr_len >= 1 && *rule->rdr_str == '/' &&
	    (msg->flags & HTTP_MSGF_XFER_LEN) &&
	    !(msg->flags & HTTP_MSGF_TE_CHNK) && !txn->req.body_len &&
	    ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL ||
	     (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL)) {
		/* keep-alive possible */
		if (!(msg->flags & HTTP_MSGF_VER_11)) {
			if (unlikely(txn->flags & TX_USE_PX_CONN)) {
				memcpy(trash.str + trash.len, "\r\nProxy-Connection: keep-alive", 30);
				trash.len += 30;
			} else {
				memcpy(trash.str + trash.len, "\r\nConnection: keep-alive", 24);
				trash.len += 24;
			}
		}
		memcpy(trash.str + trash.len, "\r\n\r\n", 4);
		trash.len += 4;
		bo_inject(txn->rsp.chn, trash.str, trash.len);
		/* "eat" the request */
		bi_fast_delete(txn->req.chn->buf, msg->sov);
		msg->sov = 0;
		txn->req.chn->analysers = AN_REQ_HTTP_XFER_BODY;
		s->rep->analysers = AN_RES_HTTP_XFER_BODY;
		txn->req.msg_state = HTTP_MSG_CLOSED;
		txn->rsp.msg_state = HTTP_MSG_DONE;
	} else {
		/* keep-alive not possible */
		if (unlikely(txn->flags & TX_USE_PX_CONN)) {
			memcpy(trash.str + trash.len, "\r\nProxy-Connection: close\r\n\r\n", 29);
			trash.len += 29;
		} else {
			memcpy(trash.str + trash.len, "\r\nConnection: close\r\n\r\n", 23);
			trash.len += 23;
		}
		stream_int_retnclose(txn->req.chn->prod, &trash);
		txn->req.chn->analysers = 0;
	}

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_R;

	return 1;
}