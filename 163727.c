void check_response_for_cacheability(struct session *t, struct channel *rtr)
{
	struct http_txn *txn = &t->txn;
	char *p1, *p2;

	char *cur_ptr, *cur_end, *cur_next;
	int cur_idx;

	if (!(txn->flags & TX_CACHEABLE))
		return;

	/* Iterate through the headers.
	 * we start with the start line.
	 */
	cur_idx = 0;
	cur_next = rtr->buf->p + hdr_idx_first_pos(&txn->hdr_idx);

	while ((cur_idx = txn->hdr_idx.v[cur_idx].next)) {
		struct hdr_idx_elem *cur_hdr;
		int val;

		cur_hdr  = &txn->hdr_idx.v[cur_idx];
		cur_ptr  = cur_next;
		cur_end  = cur_ptr + cur_hdr->len;
		cur_next = cur_end + cur_hdr->cr + 1;

		/* We have one full header between cur_ptr and cur_end, and the
		 * next header starts at cur_next. We're only interested in
		 * "Cookie:" headers.
		 */

		val = http_header_match2(cur_ptr, cur_end, "Pragma", 6);
		if (val) {
			if ((cur_end - (cur_ptr + val) >= 8) &&
			    strncasecmp(cur_ptr + val, "no-cache", 8) == 0) {
				txn->flags &= ~TX_CACHEABLE & ~TX_CACHE_COOK;
				return;
			}
		}

		val = http_header_match2(cur_ptr, cur_end, "Cache-control", 13);
		if (!val)
			continue;

		/* OK, right now we know we have a cache-control header at cur_ptr */

		p1 = cur_ptr + val; /* first non-space char after 'cache-control:' */

		if (p1 >= cur_end)	/* no more info */
			continue;

		/* p1 is at the beginning of the value */
		p2 = p1;

		while (p2 < cur_end && *p2 != '=' && *p2 != ',' && !isspace((unsigned char)*p2))
			p2++;

		/* we have a complete value between p1 and p2 */
		if (p2 < cur_end && *p2 == '=') {
			/* we have something of the form no-cache="set-cookie" */
			if ((cur_end - p1 >= 21) &&
			    strncasecmp(p1, "no-cache=\"set-cookie", 20) == 0
			    && (p1[20] == '"' || p1[20] == ','))
				txn->flags &= ~TX_CACHE_COOK;
			continue;
		}

		/* OK, so we know that either p2 points to the end of string or to a comma */
		if (((p2 - p1 ==  7) && strncasecmp(p1, "private", 7) == 0) ||
		    ((p2 - p1 ==  8) && strncasecmp(p1, "no-store", 8) == 0) ||
		    ((p2 - p1 ==  9) && strncasecmp(p1, "max-age=0", 9) == 0) ||
		    ((p2 - p1 == 10) && strncasecmp(p1, "s-maxage=0", 10) == 0)) {
			txn->flags &= ~TX_CACHEABLE & ~TX_CACHE_COOK;
			return;
		}

		if ((p2 - p1 ==  6) && strncasecmp(p1, "public", 6) == 0) {
			txn->flags |= TX_CACHEABLE | TX_CACHE_COOK;
			continue;
		}
	}
}