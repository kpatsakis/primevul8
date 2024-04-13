http_req_get_intercept_rule(struct proxy *px, struct list *rules, struct session *s, struct http_txn *txn)
{
	struct http_req_rule *rule;
	struct hdr_ctx ctx;

	list_for_each_entry(rule, rules, list) {
		if (rule->action >= HTTP_REQ_ACT_MAX)
			continue;

		/* check optional condition */
		if (rule->cond) {
			int ret;

			ret = acl_exec_cond(rule->cond, px, s, txn, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);
			ret = acl_pass(ret);

			if (rule->cond->pol == ACL_COND_UNLESS)
				ret = !ret;

			if (!ret) /* condition not matched */
				continue;
		}


		switch (rule->action) {
		case HTTP_REQ_ACT_ALLOW:
			return NULL; /* "allow" rules are OK */

		case HTTP_REQ_ACT_DENY:
			txn->flags |= TX_CLDENY;
			return rule;

		case HTTP_REQ_ACT_TARPIT:
			txn->flags |= TX_CLTARPIT;
			return rule;

		case HTTP_REQ_ACT_AUTH:
			return rule;

		case HTTP_REQ_ACT_REDIR:
			return rule;

		case HTTP_REQ_ACT_SET_HDR:
			ctx.idx = 0;
			/* remove all occurrences of the header */
			while (http_find_header2(rule->arg.hdr_add.name, rule->arg.hdr_add.name_len,
						 txn->req.chn->buf->p, &txn->hdr_idx, &ctx)) {
				http_remove_header2(&txn->req, &txn->hdr_idx, &ctx);
			}
			/* now fall through to header addition */

		case HTTP_REQ_ACT_ADD_HDR:
			chunk_printf(&trash, "%s: ", rule->arg.hdr_add.name);
			memcpy(trash.str, rule->arg.hdr_add.name, rule->arg.hdr_add.name_len);
			trash.len = rule->arg.hdr_add.name_len;
			trash.str[trash.len++] = ':';
			trash.str[trash.len++] = ' ';
			trash.len += build_logline(s, trash.str + trash.len, trash.size - trash.len, &rule->arg.hdr_add.fmt);
			http_header_add_tail2(&txn->req, &txn->hdr_idx, trash.str, trash.len);
			break;
		}
	}

	/* we reached the end of the rules, nothing to report */
	return NULL;
}