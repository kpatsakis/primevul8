int http_eval_after_res_rules(struct stream *s)
{
	struct list *def_rules, *rules;
	struct session *sess = s->sess;
	enum rule_result ret = HTTP_RULE_RES_CONT;

	/* Eval after-response ruleset only if the reply is not const */
	if (s->txn->flags & TX_CONST_REPLY)
		goto end;

	/* prune the request variables if not already done and swap to the response variables. */
	if (s->vars_reqres.scope != SCOPE_RES) {
		if (!LIST_ISEMPTY(&s->vars_reqres.head))
			vars_prune(&s->vars_reqres, s->sess, s);
		vars_init_head(&s->vars_reqres, SCOPE_RES);
	}

	def_rules = (s->be->defpx ? &s->be->defpx->http_after_res_rules : NULL);
	rules = &s->be->http_after_res_rules;

	ret = http_res_get_intercept_rule(s->be, def_rules, rules, s);
	if ((ret == HTTP_RULE_RES_CONT || ret == HTTP_RULE_RES_STOP) && sess->fe != s->be) {
		def_rules = ((sess->fe->defpx && sess->fe->defpx != s->be->defpx) ? &sess->fe->defpx->http_after_res_rules : NULL);
		rules = &sess->fe->http_after_res_rules;
		ret = http_res_get_intercept_rule(sess->fe, def_rules, rules, s);
	}

  end:
	/* All other codes than CONTINUE, STOP or DONE are forbidden */
	return (ret == HTTP_RULE_RES_CONT || ret == HTTP_RULE_RES_STOP || ret == HTTP_RULE_RES_DONE);
}