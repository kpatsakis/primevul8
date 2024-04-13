int http_wait_for_request(struct stream *s, struct channel *req, int an_bit)
{

	/*
	 * We will analyze a complete HTTP request to check the its syntax.
	 *
	 * Once the start line and all headers are received, we may perform a
	 * capture of the error (if any), and we will set a few fields. We also
	 * check for monitor-uri, logging and finally headers capture.
	 */
	struct session *sess = s->sess;
	struct http_txn *txn = s->txn;
	struct http_msg *msg = &txn->req;
	struct htx *htx;
	struct htx_sl *sl;

	DBG_TRACE_ENTER(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn, msg);

	if (unlikely(!IS_HTX_STRM(s))) {
		/* It is only possible when a TCP stream is upgrade to HTTP.
		 * There is a transition period during which there is no
		 * data. The stream is still in raw mode and SF_IGNORE flag is
		 * still set. When this happens, the new mux is responsible to
		 * handle all errors. Thus we may leave immediately.
		 */
		BUG_ON(!(s->flags & SF_IGNORE) || !c_empty(&s->req));

		/* Don't connect for now */
		channel_dont_connect(req);

		/* A SHUTR at this stage means we are performing a "destructive"
		 * HTTP upgrade (TCP>H2). In this case, we can leave.
		 */
		if (req->flags & CF_SHUTR) {
			s->logs.logwait = 0;
                        s->logs.level = 0;
			channel_abort(&s->req);
			channel_abort(&s->res);
			req->analysers &= AN_REQ_FLT_END;
			req->analyse_exp = TICK_ETERNITY;
			DBG_TRACE_LEAVE(STRM_EV_STRM_ANA, s);
			return 1;
		}
		DBG_TRACE_LEAVE(STRM_EV_STRM_ANA, s);
		return 0;
	}

	htx = htxbuf(&req->buf);

	/* Parsing errors are caught here */
	if (htx->flags & (HTX_FL_PARSING_ERROR|HTX_FL_PROCESSING_ERROR)) {
		stream_inc_http_req_ctr(s);
		proxy_inc_fe_req_ctr(sess->listener, sess->fe);
		if (htx->flags & HTX_FL_PARSING_ERROR) {
			stream_inc_http_err_ctr(s);
			goto return_bad_req;
		}
		else
			goto return_int_err;
	}

	/* we're speaking HTTP here, so let's speak HTTP to the client */
	s->srv_error = http_return_srv_error;

	msg->msg_state = HTTP_MSG_BODY;
	stream_inc_http_req_ctr(s);
	proxy_inc_fe_req_ctr(sess->listener, sess->fe); /* one more valid request for this FE */

	/* kill the pending keep-alive timeout */
	req->analyse_exp = TICK_ETERNITY;

	BUG_ON(htx_get_first_type(htx) != HTX_BLK_REQ_SL);
	sl = http_get_stline(htx);

	/* 0: we might have to print this header in debug mode */
	if (unlikely((global.mode & MODE_DEBUG) &&
		     (!(global.mode & MODE_QUIET) || (global.mode & MODE_VERBOSE)))) {
		int32_t pos;

		http_debug_stline("clireq", s, sl);

		for (pos = htx_get_first(htx); pos != -1; pos = htx_get_next(htx, pos)) {
			struct htx_blk *blk = htx_get_blk(htx, pos);
			enum htx_blk_type type = htx_get_blk_type(blk);

			if (type == HTX_BLK_EOH)
				break;
			if (type != HTX_BLK_HDR)
				continue;

			http_debug_hdr("clihdr", s,
				       htx_get_blk_name(htx, blk),
				       htx_get_blk_value(htx, blk));
		}
	}

	/*
	 * 1: identify the method and the version. Also set HTTP flags
	 */
	txn->meth = sl->info.req.meth;
	if (sl->flags & HTX_SL_F_VER_11)
                msg->flags |= HTTP_MSGF_VER_11;
	msg->flags |= HTTP_MSGF_XFER_LEN;
	if (sl->flags & HTX_SL_F_CLEN)
		msg->flags |= HTTP_MSGF_CNT_LEN;
	else if (sl->flags & HTX_SL_F_CHNK)
		msg->flags |= HTTP_MSGF_TE_CHNK;
	if (sl->flags & HTX_SL_F_BODYLESS)
		msg->flags |= HTTP_MSGF_BODYLESS;
	if (sl->flags & HTX_SL_F_CONN_UPG)
		msg->flags |= HTTP_MSGF_CONN_UPG;

	/* we can make use of server redirect on GET and HEAD */
	if (txn->meth == HTTP_METH_GET || txn->meth == HTTP_METH_HEAD)
		s->flags |= SF_REDIRECTABLE;
	else if (txn->meth == HTTP_METH_OTHER && isteqi(htx_sl_req_meth(sl), ist("PRI"))) {
		/* PRI is reserved for the HTTP/2 preface */
		goto return_bad_req;
	}

	/*
	 * 2: check if the URI matches the monitor_uri.  We have to do this for
	 * every request which gets in, because the monitor-uri is defined by
	 * the frontend. If the monitor-uri starts with a '/', the matching is
	 * done against the request's path. Otherwise, the request's uri is
	 * used. It is a workaround to let HTTP/2 health-checks work as
	 * expected.
	 */
	if (unlikely(sess->fe->monitor_uri_len != 0)) {
		const struct ist monitor_uri = ist2(sess->fe->monitor_uri,
		                                    sess->fe->monitor_uri_len);
		struct http_uri_parser parser = http_uri_parser_init(htx_sl_req_uri(sl));

		if ((istptr(monitor_uri)[0] == '/' &&
		     isteq(http_parse_path(&parser), monitor_uri)) ||
		    isteq(htx_sl_req_uri(sl), monitor_uri)) {
			/*
			 * We have found the monitor URI
			 */
			struct acl_cond *cond;

			s->flags |= SF_MONITOR;
			_HA_ATOMIC_INC(&sess->fe->fe_counters.intercepted_req);

			/* Check if we want to fail this monitor request or not */
			list_for_each_entry(cond, &sess->fe->mon_fail_cond, list) {
				int ret = acl_exec_cond(cond, sess->fe, sess, s, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);

				ret = acl_pass(ret);
				if (cond->pol == ACL_COND_UNLESS)
					ret = !ret;

				if (ret) {
					/* we fail this request, let's return 503 service unavail */
					txn->status = 503;
					if (!(s->flags & SF_ERR_MASK))
						s->flags |= SF_ERR_LOCAL; /* we don't want a real error here */
					goto return_prx_cond;
				}
			}

			/* nothing to fail, let's reply normally */
			txn->status = 200;
			if (!(s->flags & SF_ERR_MASK))
				s->flags |= SF_ERR_LOCAL; /* we don't want a real error here */
			goto return_prx_cond;
		}
	}

	/*
	 * 3: Maybe we have to copy the original REQURI for the logs ?
	 * Note: we cannot log anymore if the request has been
	 * classified as invalid.
	 */
	if (unlikely(s->logs.logwait & LW_REQ)) {
		/* we have a complete HTTP request that we must log */
		if ((txn->uri = pool_alloc(pool_head_requri)) != NULL) {
			size_t len;

			len = http_fmt_req_line(sl, txn->uri, global.tune.requri_len - 1);
			txn->uri[len] = 0;

			if (!(s->logs.logwait &= ~(LW_REQ|LW_INIT)))
				s->do_log(s);
		} else {
			ha_alert("HTTP logging : out of memory.\n");
		}
	}

	/* if the frontend has "option http-use-proxy-header", we'll check if
	 * we have what looks like a proxied connection instead of a connection,
	 * and in this case set the TX_USE_PX_CONN flag to use Proxy-connection.
	 * Note that this is *not* RFC-compliant, however browsers and proxies
	 * happen to do that despite being non-standard :-(
	 * We consider that a request not beginning with either '/' or '*' is
	 * a proxied connection, which covers both "scheme://location" and
	 * CONNECT ip:port.
	 */
	if ((sess->fe->options2 & PR_O2_USE_PXHDR) &&
	    *HTX_SL_REQ_UPTR(sl) != '/' && *HTX_SL_REQ_UPTR(sl) != '*')
		txn->flags |= TX_USE_PX_CONN;

	/* 5: we may need to capture headers */
	if (unlikely((s->logs.logwait & LW_REQHDR) && s->req_cap))
		http_capture_headers(htx, s->req_cap, sess->fe->req_cap);

	/* we may have to wait for the request's body */
	if (s->be->options & PR_O_WREQ_BODY)
		req->analysers |= AN_REQ_HTTP_BODY;

	/*
	 * RFC7234#4:
	 *   A cache MUST write through requests with methods
	 *   that are unsafe (Section 4.2.1 of [RFC7231]) to
	 *   the origin server; i.e., a cache is not allowed
	 *   to generate a reply to such a request before
	 *   having forwarded the request and having received
	 *   a corresponding response.
	 *
	 * RFC7231#4.2.1:
	 *   Of the request methods defined by this
	 *   specification, the GET, HEAD, OPTIONS, and TRACE
	 *   methods are defined to be safe.
	 */
	if (likely(txn->meth == HTTP_METH_GET ||
		   txn->meth == HTTP_METH_HEAD ||
		   txn->meth == HTTP_METH_OPTIONS ||
		   txn->meth == HTTP_METH_TRACE))
		txn->flags |= TX_CACHEABLE | TX_CACHE_COOK;

	/* end of job, return OK */
	req->analysers &= ~an_bit;
	req->analyse_exp = TICK_ETERNITY;

	DBG_TRACE_LEAVE(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
	return 1;

 return_int_err:
	txn->status = 500;
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_INTERNAL;
	_HA_ATOMIC_INC(&sess->fe->fe_counters.internal_errors);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->internal_errors);
	goto return_prx_cond;

 return_bad_req:
	txn->status = 400;
	_HA_ATOMIC_INC(&sess->fe->fe_counters.failed_req);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->failed_req);
	/* fall through */

 return_prx_cond:
	http_reply_and_close(s, txn->status, http_error_message(s));

	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_PRXCOND;
	if (!(s->flags & SF_FINST_MASK))
		s->flags |= SF_FINST_R;

	DBG_TRACE_DEVEL("leaving on error",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
	return 0;
}