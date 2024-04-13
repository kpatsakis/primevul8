int apply_filter_to_req_headers(struct session *t, struct channel *req, struct hdr_exp *exp)
{
	char term;
	char *cur_ptr, *cur_end, *cur_next;
	int cur_idx, old_idx, last_hdr;
	struct http_txn *txn = &t->txn;
	struct hdr_idx_elem *cur_hdr;
	int delta;

	last_hdr = 0;

	cur_next = req->buf->p + hdr_idx_first_pos(&txn->hdr_idx);
	old_idx = 0;

	while (!last_hdr) {
		if (unlikely(txn->flags & (TX_CLDENY | TX_CLTARPIT)))
			return 1;
		else if (unlikely(txn->flags & TX_CLALLOW) &&
			 (exp->action == ACT_ALLOW ||
			  exp->action == ACT_DENY ||
			  exp->action == ACT_TARPIT))
			return 0;

		cur_idx = txn->hdr_idx.v[old_idx].next;
		if (!cur_idx)
			break;

		cur_hdr  = &txn->hdr_idx.v[cur_idx];
		cur_ptr  = cur_next;
		cur_end  = cur_ptr + cur_hdr->len;
		cur_next = cur_end + cur_hdr->cr + 1;

		/* Now we have one header between cur_ptr and cur_end,
		 * and the next header starts at cur_next.
		 */

		/* The annoying part is that pattern matching needs
		 * that we modify the contents to null-terminate all
		 * strings before testing them.
		 */

		term = *cur_end;
		*cur_end = '\0';

		if (regexec(exp->preg, cur_ptr, MAX_MATCH, pmatch, 0) == 0) {
			switch (exp->action) {
			case ACT_SETBE:
				/* It is not possible to jump a second time.
				 * FIXME: should we return an HTTP/500 here so that
				 * the admin knows there's a problem ?
				 */
				if (t->be != t->fe)
					break;

				/* Swithing Proxy */
				session_set_backend(t, (struct proxy *)exp->replace);
				last_hdr = 1;
				break;

			case ACT_ALLOW:
				txn->flags |= TX_CLALLOW;
				last_hdr = 1;
				break;

			case ACT_DENY:
				txn->flags |= TX_CLDENY;
				last_hdr = 1;

				t->fe->fe_counters.denied_req++;
				if (t->fe != t->be)
					t->be->be_counters.denied_req++;
				if (t->listener->counters)
					t->listener->counters->denied_req++;

				break;

			case ACT_TARPIT:
				txn->flags |= TX_CLTARPIT;
				last_hdr = 1;

				t->fe->fe_counters.denied_req++;
				if (t->fe != t->be)
					t->be->be_counters.denied_req++;
				if (t->listener->counters)
					t->listener->counters->denied_req++;

				break;

			case ACT_REPLACE:
				trash.len = exp_replace(trash.str, cur_ptr, exp->replace, pmatch);
				delta = buffer_replace2(req->buf, cur_ptr, cur_end, trash.str, trash.len);
				/* FIXME: if the user adds a newline in the replacement, the
				 * index will not be recalculated for now, and the new line
				 * will not be counted as a new header.
				 */

				cur_end += delta;
				cur_next += delta;
				cur_hdr->len += delta;
				http_msg_move_end(&txn->req, delta);
				break;

			case ACT_REMOVE:
				delta = buffer_replace2(req->buf, cur_ptr, cur_next, NULL, 0);
				cur_next += delta;

				http_msg_move_end(&txn->req, delta);
				txn->hdr_idx.v[old_idx].next = cur_hdr->next;
				txn->hdr_idx.used--;
				cur_hdr->len = 0;
				cur_end = NULL; /* null-term has been rewritten */
				cur_idx = old_idx;
				break;

			}
		}
		if (cur_end)
			*cur_end = term; /* restore the string terminator */

		/* keep the link from this header to next one in case of later
		 * removal of next header.
		 */
		old_idx = cur_idx;
	}
	return 0;
}