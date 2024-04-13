int http_handle_stats(struct session *s, struct channel *req)
{
	struct stats_admin_rule *stats_admin_rule;
	struct stream_interface *si = s->rep->prod;
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &txn->req;
	struct uri_auth *uri = s->be->uri_auth;

	/* now check whether we have some admin rules for this request */
	list_for_each_entry(stats_admin_rule, &s->be->uri_auth->admin_rules, list) {
		int ret = 1;

		if (stats_admin_rule->cond) {
			ret = acl_exec_cond(stats_admin_rule->cond, s->be, s, &s->txn, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);
			ret = acl_pass(ret);
			if (stats_admin_rule->cond->pol == ACL_COND_UNLESS)
				ret = !ret;
		}

		if (ret) {
			/* no rule, or the rule matches */
			s->rep->prod->applet.ctx.stats.flags |= STAT_ADMIN;
			break;
		}
	}

	/* Was the status page requested with a POST ? */
	if (unlikely(txn->meth == HTTP_METH_POST)) {
		if (si->applet.ctx.stats.flags & STAT_ADMIN) {
			if (msg->msg_state < HTTP_MSG_100_SENT) {
				/* If we have HTTP/1.1 and Expect: 100-continue, then we must
				 * send an HTTP/1.1 100 Continue intermediate response.
				 */
				if (msg->flags & HTTP_MSGF_VER_11) {
					struct hdr_ctx ctx;
					ctx.idx = 0;
					/* Expect is allowed in 1.1, look for it */
					if (http_find_header2("Expect", 6, req->buf->p, &txn->hdr_idx, &ctx) &&
					    unlikely(ctx.vlen == 12 && strncasecmp(ctx.line+ctx.val, "100-continue", 12) == 0)) {
						bo_inject(s->rep, http_100_chunk.str, http_100_chunk.len);
					}
				}
				msg->msg_state = HTTP_MSG_100_SENT;
				s->logs.tv_request = now;  /* update the request timer to reflect full request */
			}
			if (!http_process_req_stat_post(si, txn, req))
				return 0;   /* we need more data */
		}
		else
			si->applet.ctx.stats.st_code = STAT_STATUS_DENY;

		/* We don't want to land on the posted stats page because a refresh will
		 * repost the data. We don't want this to happen on accident so we redirect
		 * the browse to the stats page with a GET.
		 */
		chunk_printf(&trash,
		             "HTTP/1.1 303 See Other\r\n"
		             "Cache-Control: no-cache\r\n"
		             "Content-Type: text/plain\r\n"
		             "Connection: close\r\n"
		             "Location: %s;st=%s\r\n"
		             "\r\n",
		             uri->uri_prefix,
		             ((si->applet.ctx.stats.st_code > STAT_STATUS_INIT) &&
		              (si->applet.ctx.stats.st_code < STAT_STATUS_SIZE) &&
		              stat_status_codes[si->applet.ctx.stats.st_code]) ?
		             stat_status_codes[si->applet.ctx.stats.st_code] :
		             stat_status_codes[STAT_STATUS_UNKN]);

		s->txn.status = 303;
		s->logs.tv_request = now;
		stream_int_retnclose(req->prod, &trash);
		s->target = &http_stats_applet.obj_type; /* just for logging the applet name */

		if (s->fe == s->be) /* report it if the request was intercepted by the frontend */
			s->fe->fe_counters.intercepted_req++;

		if (!(s->flags & SN_ERR_MASK))      // this is not really an error but it is
			s->flags |= SN_ERR_PRXCOND; // to mark that it comes from the proxy
		if (!(s->flags & SN_FINST_MASK))
			s->flags |= SN_FINST_R;
		req->analysers = 0;
		return 1;
	}

	/* OK, let's go on now */

	chunk_printf(&trash,
	             "HTTP/1.0 200 OK\r\n"
	             "Cache-Control: no-cache\r\n"
	             "Connection: close\r\n"
	             "Content-Type: %s\r\n",
	             (si->applet.ctx.stats.flags & STAT_FMT_HTML) ? "text/html" : "text/plain");

	if (uri->refresh > 0 && !(si->applet.ctx.stats.flags & STAT_NO_REFRESH))
		chunk_appendf(&trash, "Refresh: %d\r\n",
		              uri->refresh);

	chunk_appendf(&trash, "\r\n");

	s->txn.status = 200;
	s->logs.tv_request = now;

	if (s->fe == s->be) /* report it if the request was intercepted by the frontend */
		s->fe->fe_counters.intercepted_req++;

	if (!(s->flags & SN_ERR_MASK))      // this is not really an error but it is
		s->flags |= SN_ERR_PRXCOND; // to mark that it comes from the proxy
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_R;

	if (s->txn.meth == HTTP_METH_HEAD) {
		/* that's all we return in case of HEAD request, so let's immediately close. */
		stream_int_retnclose(req->prod, &trash);
		s->target = &http_stats_applet.obj_type; /* just for logging the applet name */
		req->analysers = 0;
		return 1;
	}

	/* OK, push the response and hand over to the stats I/O handler */
	bi_putchk(s->rep, &trash);

	s->task->nice = -32; /* small boost for HTTP statistics */
	stream_int_register_handler(s->rep->prod, &http_stats_applet);
	s->target = s->rep->prod->conn->target; // for logging only
	s->rep->prod->conn->xprt_ctx = s;
	s->rep->prod->applet.st0 = s->rep->prod->applet.st1 = 0;
	req->analysers = 0;
	return 1;
}