static enum rule_result http_req_get_intercept_rule(struct proxy *px, struct list *def_rules,
						    struct list *rules, struct stream *s)
{
	struct session *sess = strm_sess(s);
	struct http_txn *txn = s->txn;
	struct act_rule *rule;
	enum rule_result rule_ret = HTTP_RULE_RES_CONT;
	int act_opts = 0;

	/* If "the current_rule_list" match the executed rule list, we are in
	 * resume condition. If a resume is needed it is always in the action
	 * and never in the ACL or converters. In this case, we initialise the
	 * current rule, and go to the action execution point.
	 */
	if (s->current_rule) {
		rule = s->current_rule;
		s->current_rule = NULL;
		if (s->current_rule_list == rules || (def_rules && s->current_rule_list == def_rules))
			goto resume_execution;
	}
	s->current_rule_list = ((!def_rules || s->current_rule_list == def_rules) ? rules : def_rules);

  restart:
	/* start the ruleset evaluation in strict mode */
	txn->req.flags &= ~HTTP_MSGF_SOFT_RW;

	list_for_each_entry(rule, s->current_rule_list, list) {
		/* check optional condition */
		if (rule->cond) {
			int ret;

			ret = acl_exec_cond(rule->cond, px, sess, s, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);
			ret = acl_pass(ret);

			if (rule->cond->pol == ACL_COND_UNLESS)
				ret = !ret;

			if (!ret) /* condition not matched */
				continue;
		}

		act_opts |= ACT_OPT_FIRST;
  resume_execution:
		if (rule->kw->flags & KWF_EXPERIMENTAL)
			mark_tainted(TAINTED_ACTION_EXP_EXECUTED);

		/* Always call the action function if defined */
		if (rule->action_ptr) {
			if ((s->req.flags & CF_READ_ERROR) ||
			    ((s->req.flags & (CF_SHUTR|CF_READ_NULL)) &&
			     (px->options & PR_O_ABRT_CLOSE)))
				act_opts |= ACT_OPT_FINAL;

			switch (rule->action_ptr(rule, px, sess, s, act_opts)) {
				case ACT_RET_CONT:
					break;
				case ACT_RET_STOP:
					rule_ret = HTTP_RULE_RES_STOP;
					goto end;
				case ACT_RET_YIELD:
					s->current_rule = rule;
					rule_ret = HTTP_RULE_RES_YIELD;
					goto end;
				case ACT_RET_ERR:
					rule_ret = HTTP_RULE_RES_ERROR;
					goto end;
				case ACT_RET_DONE:
					rule_ret = HTTP_RULE_RES_DONE;
					goto end;
				case ACT_RET_DENY:
					if (txn->status == -1)
						txn->status = 403;
					rule_ret = HTTP_RULE_RES_DENY;
					goto end;
				case ACT_RET_ABRT:
					rule_ret = HTTP_RULE_RES_ABRT;
					goto end;
				case ACT_RET_INV:
					rule_ret = HTTP_RULE_RES_BADREQ;
					goto end;
			}
			continue; /* eval the next rule */
		}

		/* If not action function defined, check for known actions */
		switch (rule->action) {
			case ACT_ACTION_ALLOW:
				rule_ret = HTTP_RULE_RES_STOP;
				goto end;

			case ACT_ACTION_DENY:
				txn->status = rule->arg.http_reply->status;
				txn->http_reply = rule->arg.http_reply;
				rule_ret = HTTP_RULE_RES_DENY;
				goto end;

			case ACT_HTTP_REQ_TARPIT:
				txn->flags |= TX_CLTARPIT;
				txn->status = rule->arg.http_reply->status;
				txn->http_reply = rule->arg.http_reply;
				rule_ret = HTTP_RULE_RES_DENY;
				goto end;

			case ACT_HTTP_REDIR: {
				int ret = http_apply_redirect_rule(rule->arg.redir, s, txn);

				if (ret == 2) // 2 == skip
					break;

				rule_ret = ret ? HTTP_RULE_RES_ABRT : HTTP_RULE_RES_ERROR;
				goto end;
			}

			/* other flags exists, but normally, they never be matched. */
			default:
				break;
		}
	}

	if (def_rules && s->current_rule_list == def_rules) {
		s->current_rule_list = rules;
		goto restart;
	}

  end:
	/* if the ruleset evaluation is finished reset the strict mode */
	if (rule_ret != HTTP_RULE_RES_YIELD)
		txn->req.flags &= ~HTTP_MSGF_SOFT_RW;

	/* we reached the end of the rules, nothing to report */
	return rule_ret;
}