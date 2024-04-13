int http_process_req_common(struct session *s, struct channel *req, int an_bit, struct proxy *px)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &txn->req;
	struct acl_cond *cond;
	struct http_req_rule *http_req_last_rule = NULL;
	struct redirect_rule *rule;
	struct cond_wordlist *wl;
	int do_stats;

	if (unlikely(msg->msg_state < HTTP_MSG_BODY)) {
		/* we need more data */
		channel_dont_connect(req);
		return 0;
	}

	req->analysers &= ~an_bit;
	req->analyse_exp = TICK_ETERNITY;

	DPRINTF(stderr,"[%u] %s: session=%p b=%p, exp(r,w)=%u,%u bf=%08x bh=%d analysers=%02x\n",
		now_ms, __FUNCTION__,
		s,
		req,
		req->rex, req->wex,
		req->flags,
		req->buf->i,
		req->analysers);

	/* first check whether we have some ACLs set to block this request */
	list_for_each_entry(cond, &px->block_cond, list) {
		int ret = acl_exec_cond(cond, px, s, txn, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);

		ret = acl_pass(ret);
		if (cond->pol == ACL_COND_UNLESS)
			ret = !ret;

		if (ret) {
			txn->status = 403;
			/* let's log the request time */
			s->logs.tv_request = now;
			stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_403));
			session_inc_http_err_ctr(s);
			goto return_prx_cond;
		}
	}

	/* just in case we have some per-backend tracking */
	session_inc_be_http_req_ctr(s);

	/* evaluate http-request rules */
	http_req_last_rule = http_req_get_intercept_rule(px, &px->http_req_rules, s, txn);

	/* evaluate stats http-request rules only if http-request is OK */
	if (!http_req_last_rule) {
		do_stats = stats_check_uri(s->rep->prod, txn, px);
		if (do_stats)
			http_req_last_rule = http_req_get_intercept_rule(px, &px->uri_auth->http_req_rules, s, txn);
	}
	else
		do_stats = 0;

	/* return a 403 if either rule has blocked */
	if (txn->flags & (TX_CLDENY|TX_CLTARPIT)) {
		if (txn->flags & TX_CLDENY) {
			txn->status = 403;
			s->logs.tv_request = now;
			stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_403));
			session_inc_http_err_ctr(s);
			s->fe->fe_counters.denied_req++;
			if (an_bit == AN_REQ_HTTP_PROCESS_BE)
				s->be->be_counters.denied_req++;
			if (s->listener->counters)
				s->listener->counters->denied_req++;
			goto return_prx_cond;
		}
		/* When a connection is tarpitted, we use the tarpit timeout,
		 * which may be the same as the connect timeout if unspecified.
		 * If unset, then set it to zero because we really want it to
		 * eventually expire. We build the tarpit as an analyser.
		 */
		if (txn->flags & TX_CLTARPIT) {
			channel_erase(s->req);
			/* wipe the request out so that we can drop the connection early
			 * if the client closes first.
			 */
			channel_dont_connect(req);
			req->analysers = 0; /* remove switching rules etc... */
			req->analysers |= AN_REQ_HTTP_TARPIT;
			req->analyse_exp = tick_add_ifset(now_ms,  s->be->timeout.tarpit);
			if (!req->analyse_exp)
				req->analyse_exp = tick_add(now_ms, 0);
			session_inc_http_err_ctr(s);
			s->fe->fe_counters.denied_req++;
			if (s->fe != s->be)
				s->be->be_counters.denied_req++;
			if (s->listener->counters)
				s->listener->counters->denied_req++;
			return 1;
		}
	}

	/* try headers filters */
	if (px->req_exp != NULL) {
		if (apply_filters_to_request(s, req, px) < 0)
			goto return_bad_req;

		/* has the request been denied ? */
		if (txn->flags & TX_CLDENY) {
			/* no need to go further */
			txn->status = 403;
			/* let's log the request time */
			s->logs.tv_request = now;
			stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_403));
			session_inc_http_err_ctr(s);
			goto return_prx_cond;
		}

		/* When a connection is tarpitted, we use the tarpit timeout,
		 * which may be the same as the connect timeout if unspecified.
		 * If unset, then set it to zero because we really want it to
		 * eventually expire. We build the tarpit as an analyser.
		 */
		if (txn->flags & TX_CLTARPIT) {
			channel_erase(s->req);
			/* wipe the request out so that we can drop the connection early
			 * if the client closes first.
			 */
			channel_dont_connect(req);
			req->analysers = 0; /* remove switching rules etc... */
			req->analysers |= AN_REQ_HTTP_TARPIT;
			req->analyse_exp = tick_add_ifset(now_ms,  s->be->timeout.tarpit);
			if (!req->analyse_exp)
				req->analyse_exp = tick_add(now_ms, 0);
			session_inc_http_err_ctr(s);
			return 1;
		}
	}

	/* Until set to anything else, the connection mode is set as TUNNEL. It will
	 * only change if both the request and the config reference something else.
	 * Option httpclose by itself does not set a mode, it remains a tunnel mode
	 * in which headers are mangled. However, if another mode is set, it will
	 * affect it (eg: server-close/keep-alive + httpclose = close). Note that we
	 * avoid to redo the same work if FE and BE have the same settings (common).
	 * The method consists in checking if options changed between the two calls
	 * (implying that either one is non-null, or one of them is non-null and we
	 * are there for the first time.
	 */

	if ((!(txn->flags & TX_HDR_CONN_PRS) &&
	     (s->fe->options & (PR_O_KEEPALIVE|PR_O_SERVER_CLO|PR_O_HTTP_CLOSE|PR_O_FORCE_CLO))) ||
	    ((s->fe->options & (PR_O_KEEPALIVE|PR_O_SERVER_CLO|PR_O_HTTP_CLOSE|PR_O_FORCE_CLO)) !=
	     (s->be->options & (PR_O_KEEPALIVE|PR_O_SERVER_CLO|PR_O_HTTP_CLOSE|PR_O_FORCE_CLO)))) {
		int tmp = TX_CON_WANT_TUN;

		if ((s->fe->options|s->be->options) & PR_O_KEEPALIVE ||
		    ((s->fe->options2|s->be->options2) & PR_O2_FAKE_KA))
			tmp = TX_CON_WANT_KAL;
		if ((s->fe->options|s->be->options) & PR_O_SERVER_CLO)
			tmp = TX_CON_WANT_SCL;
		if ((s->fe->options|s->be->options) & PR_O_FORCE_CLO)
			tmp = TX_CON_WANT_CLO;

		if ((txn->flags & TX_CON_WANT_MSK) < tmp)
			txn->flags = (txn->flags & ~TX_CON_WANT_MSK) | tmp;

		if (!(txn->flags & TX_HDR_CONN_PRS)) {
			/* parse the Connection header and possibly clean it */
			int to_del = 0;
			if ((msg->flags & HTTP_MSGF_VER_11) ||
			    ((txn->flags & TX_CON_WANT_MSK) >= TX_CON_WANT_SCL &&
			     !((s->fe->options2|s->be->options2) & PR_O2_FAKE_KA)))
				to_del |= 2; /* remove "keep-alive" */
			if (!(msg->flags & HTTP_MSGF_VER_11))
				to_del |= 1; /* remove "close" */
			http_parse_connection_header(txn, msg, to_del);
		}

		/* check if client or config asks for explicit close in KAL/SCL */
		if (((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL ||
		     (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL) &&
		    ((txn->flags & TX_HDR_CONN_CLO) ||                         /* "connection: close" */
		     (!(msg->flags & HTTP_MSGF_VER_11) && !(txn->flags & TX_HDR_CONN_KAL)) || /* no "connection: k-a" in 1.0 */
		     ((s->fe->options|s->be->options) & PR_O_HTTP_CLOSE) ||    /* httpclose+any = forceclose */
		     !(msg->flags & HTTP_MSGF_XFER_LEN) ||                     /* no length known => close */
		     s->fe->state == PR_STSTOPPED))                            /* frontend is stopping */
		    txn->flags = (txn->flags & ~TX_CON_WANT_MSK) | TX_CON_WANT_CLO;
	}

	/* we can be blocked here because the request needs to be authenticated,
	 * either to pass or to access stats.
	 */
	if (http_req_last_rule && http_req_last_rule->action == HTTP_REQ_ACT_AUTH) {
		char *realm = http_req_last_rule->arg.auth.realm;

		if (!realm)
			realm = do_stats?STATS_DEFAULT_REALM:px->id;

		chunk_printf(&trash, (txn->flags & TX_USE_PX_CONN) ? HTTP_407_fmt : HTTP_401_fmt, realm);
		txn->status = 401;
		stream_int_retnclose(req->prod, &trash);
		/* on 401 we still count one error, because normal browsing
		 * won't significantly increase the counter but brute force
		 * attempts will.
		 */
		session_inc_http_err_ctr(s);
		goto return_prx_cond;
	}

	/* add request headers from the rule sets in the same order */
	list_for_each_entry(wl, &px->req_add, list) {
		if (wl->cond) {
			int ret = acl_exec_cond(wl->cond, px, s, txn, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);
			ret = acl_pass(ret);
			if (((struct acl_cond *)wl->cond)->pol == ACL_COND_UNLESS)
				ret = !ret;
			if (!ret)
				continue;
		}

		if (unlikely(http_header_add_tail(&txn->req, &txn->hdr_idx, wl->s) < 0))
			goto return_bad_req;
	}

	if (http_req_last_rule && http_req_last_rule->action == HTTP_REQ_ACT_REDIR) {
		if (!http_apply_redirect_rule(http_req_last_rule->arg.redir, s, txn))
			goto return_bad_req;
		req->analyse_exp = TICK_ETERNITY;
		return 1;
	}

	if (unlikely(do_stats)) {
		/* process the stats request now */
		if (!http_handle_stats(s, req)) {
			/* we need more data, let's come back here later */
			req->analysers |= an_bit;
			channel_dont_connect(req);
		}
		return 1;
	}

	/* check whether we have some ACLs set to redirect this request */
	list_for_each_entry(rule, &px->redirect_rules, list) {
		if (rule->cond) {
			int ret;

			ret = acl_exec_cond(rule->cond, px, s, txn, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);
			ret = acl_pass(ret);
			if (rule->cond->pol == ACL_COND_UNLESS)
				ret = !ret;
			if (!ret)
				continue;
		}
		if (!http_apply_redirect_rule(rule, s, txn))
			goto return_bad_req;

		req->analyse_exp = TICK_ETERNITY;
		return 1;
	}

	/* POST requests may be accompanied with an "Expect: 100-Continue" header.
	 * If this happens, then the data will not come immediately, so we must
	 * send all what we have without waiting. Note that due to the small gain
	 * in waiting for the body of the request, it's easier to simply put the
	 * CF_SEND_DONTWAIT flag any time. It's a one-shot flag so it will remove
	 * itself once used.
	 */
	req->flags |= CF_SEND_DONTWAIT;

	/* that's OK for us now, let's move on to next analysers */
	return 1;

 return_bad_req:
	/* We centralize bad requests processing here */
	if (unlikely(msg->msg_state == HTTP_MSG_ERROR) || msg->err_pos >= 0) {
		/* we detected a parsing error. We want to archive this request
		 * in the dedicated proxy area for later troubleshooting.
		 */
		http_capture_bad_message(&s->fe->invalid_req, s, msg, msg->msg_state, s->fe);
	}

	txn->req.msg_state = HTTP_MSG_ERROR;
	txn->status = 400;
	stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_400));

	s->fe->fe_counters.failed_req++;
	if (s->listener->counters)
		s->listener->counters->failed_req++;

 return_prx_cond:
	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_R;

	req->analysers = 0;
	req->analyse_exp = TICK_ETERNITY;
	return 0;
}