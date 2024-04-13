static int http_handle_stats(struct stream *s, struct channel *req)
{
	struct stats_admin_rule *stats_admin_rule;
	struct stream_interface *si = &s->si[1];
	struct session *sess = s->sess;
	struct http_txn *txn = s->txn;
	struct http_msg *msg = &txn->req;
	struct uri_auth *uri_auth = s->be->uri_auth;
	const char *h, *lookup, *end;
	struct appctx *appctx;
	struct htx *htx;
	struct htx_sl *sl;

	appctx = si_appctx(si);
	memset(&appctx->ctx.stats, 0, sizeof(appctx->ctx.stats));
	appctx->st1 = appctx->st2 = 0;
	appctx->ctx.stats.st_code = STAT_STATUS_INIT;
	appctx->ctx.stats.flags |= uri_auth->flags;
	appctx->ctx.stats.flags |= STAT_FMT_HTML; /* assume HTML mode by default */
	if ((msg->flags & HTTP_MSGF_VER_11) && (txn->meth != HTTP_METH_HEAD))
		appctx->ctx.stats.flags |= STAT_CHUNKED;

	htx = htxbuf(&req->buf);
	sl = http_get_stline(htx);
	lookup = HTX_SL_REQ_UPTR(sl) + uri_auth->uri_len;
	end = HTX_SL_REQ_UPTR(sl) + HTX_SL_REQ_ULEN(sl);

	for (h = lookup; h <= end - 3; h++) {
		if (memcmp(h, ";up", 3) == 0) {
			appctx->ctx.stats.flags |= STAT_HIDE_DOWN;
			break;
		}
	}

	for (h = lookup; h <= end - 9; h++) {
		if (memcmp(h, ";no-maint", 9) == 0) {
			appctx->ctx.stats.flags |= STAT_HIDE_MAINT;
			break;
		}
	}

	if (uri_auth->refresh) {
		for (h = lookup; h <= end - 10; h++) {
			if (memcmp(h, ";norefresh", 10) == 0) {
				appctx->ctx.stats.flags |= STAT_NO_REFRESH;
				break;
			}
		}
	}

	for (h = lookup; h <= end - 4; h++) {
		if (memcmp(h, ";csv", 4) == 0) {
			appctx->ctx.stats.flags &= ~(STAT_FMT_MASK|STAT_JSON_SCHM);
			break;
		}
	}

	for (h = lookup; h <= end - 6; h++) {
		if (memcmp(h, ";typed", 6) == 0) {
			appctx->ctx.stats.flags &= ~(STAT_FMT_MASK|STAT_JSON_SCHM);
			appctx->ctx.stats.flags |= STAT_FMT_TYPED;
			break;
		}
	}

	for (h = lookup; h <= end - 5; h++) {
		if (memcmp(h, ";json", 5) == 0) {
			appctx->ctx.stats.flags &= ~(STAT_FMT_MASK|STAT_JSON_SCHM);
			appctx->ctx.stats.flags |= STAT_FMT_JSON;
			break;
		}
	}

	for (h = lookup; h <= end - 12; h++) {
		if (memcmp(h, ";json-schema", 12) == 0) {
			appctx->ctx.stats.flags &= ~STAT_FMT_MASK;
			appctx->ctx.stats.flags |= STAT_JSON_SCHM;
			break;
		}
	}

	for (h = lookup; h <= end - 8; h++) {
		if (memcmp(h, ";st=", 4) == 0) {
			int i;
			h += 4;
			appctx->ctx.stats.st_code = STAT_STATUS_UNKN;
			for (i = STAT_STATUS_INIT + 1; i < STAT_STATUS_SIZE; i++) {
				if (strncmp(stat_status_codes[i], h, 4) == 0) {
					appctx->ctx.stats.st_code = i;
					break;
				}
			}
			break;
		}
	}

	appctx->ctx.stats.scope_str = 0;
	appctx->ctx.stats.scope_len = 0;
	for (h = lookup; h <= end - 8; h++) {
		if (memcmp(h, STAT_SCOPE_INPUT_NAME "=", strlen(STAT_SCOPE_INPUT_NAME) + 1) == 0) {
			int itx = 0;
			const char *h2;
			char scope_txt[STAT_SCOPE_TXT_MAXLEN + 1];
			const char *err;

			h += strlen(STAT_SCOPE_INPUT_NAME) + 1;
			h2 = h;
			appctx->ctx.stats.scope_str = h2 - HTX_SL_REQ_UPTR(sl);
			while (h < end) {
				if (*h == ';' || *h == '&' || *h == ' ')
					break;
				itx++;
				h++;
			}

			if (itx > STAT_SCOPE_TXT_MAXLEN)
				itx = STAT_SCOPE_TXT_MAXLEN;
			appctx->ctx.stats.scope_len = itx;

			/* scope_txt = search query, appctx->ctx.stats.scope_len is always <= STAT_SCOPE_TXT_MAXLEN */
			memcpy(scope_txt, h2, itx);
			scope_txt[itx] = '\0';
			err = invalid_char(scope_txt);
			if (err) {
				/* bad char in search text => clear scope */
				appctx->ctx.stats.scope_str = 0;
				appctx->ctx.stats.scope_len = 0;
			}
			break;
		}
	}

	/* now check whether we have some admin rules for this request */
	list_for_each_entry(stats_admin_rule, &uri_auth->admin_rules, list) {
		int ret = 1;

		if (stats_admin_rule->cond) {
			ret = acl_exec_cond(stats_admin_rule->cond, s->be, sess, s, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);
			ret = acl_pass(ret);
			if (stats_admin_rule->cond->pol == ACL_COND_UNLESS)
				ret = !ret;
		}

		if (ret) {
			/* no rule, or the rule matches */
			appctx->ctx.stats.flags |= STAT_ADMIN;
			break;
		}
	}

	if (txn->meth == HTTP_METH_GET || txn->meth == HTTP_METH_HEAD)
		appctx->st0 = STAT_HTTP_HEAD;
	else if (txn->meth == HTTP_METH_POST) {
		if (appctx->ctx.stats.flags & STAT_ADMIN) {
			appctx->st0 = STAT_HTTP_POST;
			if (msg->msg_state < HTTP_MSG_DATA)
				req->analysers |= AN_REQ_HTTP_BODY;
		}
		else {
			/* POST without admin level */
			appctx->ctx.stats.flags &= ~STAT_CHUNKED;
			appctx->ctx.stats.st_code = STAT_STATUS_DENY;
			appctx->st0 = STAT_HTTP_LAST;
		}
	}
	else {
		/* Unsupported method */
		appctx->ctx.stats.flags &= ~STAT_CHUNKED;
		appctx->ctx.stats.st_code = STAT_STATUS_IVAL;
		appctx->st0 = STAT_HTTP_LAST;
	}

	s->task->nice = -32; /* small boost for HTTP statistics */
	return 1;
}