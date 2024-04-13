int apply_filters_to_request(struct session *s, struct channel *req, struct proxy *px)
{
	struct http_txn *txn = &s->txn;
	struct hdr_exp *exp;

	for (exp = px->req_exp; exp; exp = exp->next) {
		int ret;

		/*
		 * The interleaving of transformations and verdicts
		 * makes it difficult to decide to continue or stop
		 * the evaluation.
		 */

		if (txn->flags & (TX_CLDENY|TX_CLTARPIT))
			break;

		if ((txn->flags & TX_CLALLOW) &&
		    (exp->action == ACT_ALLOW || exp->action == ACT_DENY ||
		     exp->action == ACT_TARPIT || exp->action == ACT_PASS))
			continue;

		/* if this filter had a condition, evaluate it now and skip to
		 * next filter if the condition does not match.
		 */
		if (exp->cond) {
			ret = acl_exec_cond(exp->cond, px, s, txn, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);
			ret = acl_pass(ret);
			if (((struct acl_cond *)exp->cond)->pol == ACL_COND_UNLESS)
				ret = !ret;

			if (!ret)
				continue;
		}

		/* Apply the filter to the request line. */
		ret = apply_filter_to_req_line(s, req, exp);
		if (unlikely(ret < 0))
			return -1;

		if (likely(ret == 0)) {
			/* The filter did not match the request, it can be
			 * iterated through all headers.
			 */
			apply_filter_to_req_headers(s, req, exp);
		}
	}
	return 0;
}