void manage_server_side_cookies(struct session *t, struct channel *res)
{
	struct http_txn *txn = &t->txn;
	struct server *srv;
	int is_cookie2;
	int cur_idx, old_idx, delta;
	char *hdr_beg, *hdr_end, *hdr_next;
	char *prev, *att_beg, *att_end, *equal, *val_beg, *val_end, *next;

	/* Iterate through the headers.
	 * we start with the start line.
	 */
	old_idx = 0;
	hdr_next = res->buf->p + hdr_idx_first_pos(&txn->hdr_idx);

	while ((cur_idx = txn->hdr_idx.v[old_idx].next)) {
		struct hdr_idx_elem *cur_hdr;
		int val;

		cur_hdr  = &txn->hdr_idx.v[cur_idx];
		hdr_beg  = hdr_next;
		hdr_end  = hdr_beg + cur_hdr->len;
		hdr_next = hdr_end + cur_hdr->cr + 1;

		/* We have one full header between hdr_beg and hdr_end, and the
		 * next header starts at hdr_next. We're only interested in
		 * "Set-Cookie" and "Set-Cookie2" headers.
		 */

		is_cookie2 = 0;
		prev = hdr_beg + 10;
		val = http_header_match2(hdr_beg, hdr_end, "Set-Cookie", 10);
		if (!val) {
			val = http_header_match2(hdr_beg, hdr_end, "Set-Cookie2", 11);
			if (!val) {
				old_idx = cur_idx;
				continue;
			}
			is_cookie2 = 1;
			prev = hdr_beg + 11;
		}

		/* OK, right now we know we have a Set-Cookie* at hdr_beg, and
		 * <prev> points to the colon.
		 */
		txn->flags |= TX_SCK_PRESENT;

		/* Maybe we only wanted to see if there was a Set-Cookie (eg:
		 * check-cache is enabled) and we are not interested in checking
		 * them. Warning, the cookie capture is declared in the frontend.
		 */
		if (t->be->cookie_name == NULL &&
		    t->be->appsession_name == NULL &&
		    t->fe->capture_name == NULL)
			return;

		/* OK so now we know we have to process this response cookie.
		 * The format of the Set-Cookie header is slightly different
		 * from the format of the Cookie header in that it does not
		 * support the comma as a cookie delimiter (thus the header
		 * cannot be folded) because the Expires attribute described in
		 * the original Netscape's spec may contain an unquoted date
		 * with a comma inside. We have to live with this because
		 * many browsers don't support Max-Age and some browsers don't
		 * support quoted strings. However the Set-Cookie2 header is
		 * clean.
		 *
		 * We have to keep multiple pointers in order to support cookie
		 * removal at the beginning, middle or end of header without
		 * corrupting the header (in case of set-cookie2). A special
		 * pointer, <scav> points to the beginning of the set-cookie-av
		 * fields after the first semi-colon. The <next> pointer points
		 * either to the end of line (set-cookie) or next unquoted comma
		 * (set-cookie2). All of these headers are valid :
		 *
		 * Set-Cookie:    NAME1  =  VALUE 1  ; Secure; Path="/"\r\n
		 * Set-Cookie:NAME=VALUE; Secure; Expires=Thu, 01-Jan-1970 00:00:01 GMT\r\n
		 * Set-Cookie: NAME = VALUE ; Secure; Expires=Thu, 01-Jan-1970 00:00:01 GMT\r\n
		 * Set-Cookie2: NAME1 = VALUE 1 ; Max-Age=0, NAME2=VALUE2; Discard\r\n
		 * |          | |   | | |     | |          |                      |
		 * |          | |   | | |     | |          +-> next    hdr_end <--+
		 * |          | |   | | |     | +------------> scav
		 * |          | |   | | |     +--------------> val_end
		 * |          | |   | | +--------------------> val_beg
		 * |          | |   | +----------------------> equal
		 * |          | |   +------------------------> att_end
		 * |          | +----------------------------> att_beg
		 * |          +------------------------------> prev
		 * +-----------------------------------------> hdr_beg
		 */

		for (; prev < hdr_end; prev = next) {
			/* Iterate through all cookies on this line */

			/* find att_beg */
			att_beg = prev + 1;
			while (att_beg < hdr_end && http_is_spht[(unsigned char)*att_beg])
				att_beg++;

			/* find att_end : this is the first character after the last non
			 * space before the equal. It may be equal to hdr_end.
			 */
			equal = att_end = att_beg;

			while (equal < hdr_end) {
				if (*equal == '=' || *equal == ';' || (is_cookie2 && *equal == ','))
					break;
				if (http_is_spht[(unsigned char)*equal++])
					continue;
				att_end = equal;
			}

			/* here, <equal> points to '=', a delimitor or the end. <att_end>
			 * is between <att_beg> and <equal>, both may be identical.
			 */

			/* look for end of cookie if there is an equal sign */
			if (equal < hdr_end && *equal == '=') {
				/* look for the beginning of the value */
				val_beg = equal + 1;
				while (val_beg < hdr_end && http_is_spht[(unsigned char)*val_beg])
					val_beg++;

				/* find the end of the value, respecting quotes */
				next = find_cookie_value_end(val_beg, hdr_end);

				/* make val_end point to the first white space or delimitor after the value */
				val_end = next;
				while (val_end > val_beg && http_is_spht[(unsigned char)*(val_end - 1)])
					val_end--;
			} else {
				/* <equal> points to next comma, semi-colon or EOL */
				val_beg = val_end = next = equal;
			}

			if (next < hdr_end) {
				/* Set-Cookie2 supports multiple cookies, and <next> points to
				 * a colon or semi-colon before the end. So skip all attr-value
				 * pairs and look for the next comma. For Set-Cookie, since
				 * commas are permitted in values, skip to the end.
				 */
				if (is_cookie2)
					next = find_hdr_value_end(next, hdr_end);
				else
					next = hdr_end;
			}

			/* Now everything is as on the diagram above */

			/* Ignore cookies with no equal sign */
			if (equal == val_end)
				continue;

			/* If there are spaces around the equal sign, we need to
			 * strip them otherwise we'll get trouble for cookie captures,
			 * or even for rewrites. Since this happens extremely rarely,
			 * it does not hurt performance.
			 */
			if (unlikely(att_end != equal || val_beg > equal + 1)) {
				int stripped_before = 0;
				int stripped_after = 0;

				if (att_end != equal) {
					stripped_before = buffer_replace2(res->buf, att_end, equal, NULL, 0);
					equal   += stripped_before;
					val_beg += stripped_before;
				}

				if (val_beg > equal + 1) {
					stripped_after = buffer_replace2(res->buf, equal + 1, val_beg, NULL, 0);
					val_beg += stripped_after;
					stripped_before += stripped_after;
				}

				val_end      += stripped_before;
				next         += stripped_before;
				hdr_end      += stripped_before;
				hdr_next     += stripped_before;
				cur_hdr->len += stripped_before;
				http_msg_move_end(&txn->rsp, stripped_before);
			}

			/* First, let's see if we want to capture this cookie. We check
			 * that we don't already have a server side cookie, because we
			 * can only capture one. Also as an optimisation, we ignore
			 * cookies shorter than the declared name.
			 */
			if (t->fe->capture_name != NULL &&
			    txn->srv_cookie == NULL &&
			    (val_end - att_beg >= t->fe->capture_namelen) &&
			    memcmp(att_beg, t->fe->capture_name, t->fe->capture_namelen) == 0) {
				int log_len = val_end - att_beg;
				if ((txn->srv_cookie = pool_alloc2(pool2_capture)) == NULL) {
					Alert("HTTP logging : out of memory.\n");
				}
				else {
					if (log_len > t->fe->capture_len)
						log_len = t->fe->capture_len;
					memcpy(txn->srv_cookie, att_beg, log_len);
					txn->srv_cookie[log_len] = 0;
				}
			}

			srv = objt_server(t->target);
			/* now check if we need to process it for persistence */
			if (!(t->flags & SN_IGNORE_PRST) &&
			    (att_end - att_beg == t->be->cookie_len) && (t->be->cookie_name != NULL) &&
			    (memcmp(att_beg, t->be->cookie_name, att_end - att_beg) == 0)) {
				/* assume passive cookie by default */
				txn->flags &= ~TX_SCK_MASK;
				txn->flags |= TX_SCK_FOUND;
			
				/* If the cookie is in insert mode on a known server, we'll delete
				 * this occurrence because we'll insert another one later.
				 * We'll delete it too if the "indirect" option is set and we're in
				 * a direct access.
				 */
				if (t->be->ck_opts & PR_CK_PSV) {
					/* The "preserve" flag was set, we don't want to touch the
					 * server's cookie.
					 */
				}
				else if ((srv && (t->be->ck_opts & PR_CK_INS)) ||
				    ((t->flags & SN_DIRECT) && (t->be->ck_opts & PR_CK_IND))) {
					/* this cookie must be deleted */
					if (*prev == ':' && next == hdr_end) {
						/* whole header */
						delta = buffer_replace2(res->buf, hdr_beg, hdr_next, NULL, 0);
						txn->hdr_idx.v[old_idx].next = cur_hdr->next;
						txn->hdr_idx.used--;
						cur_hdr->len = 0;
						cur_idx = old_idx;
						hdr_next += delta;
						http_msg_move_end(&txn->rsp, delta);
						/* note: while both invalid now, <next> and <hdr_end>
						 * are still equal, so the for() will stop as expected.
						 */
					} else {
						/* just remove the value */
						int delta = del_hdr_value(res->buf, &prev, next);
						next      = prev;
						hdr_end  += delta;
						hdr_next += delta;
						cur_hdr->len += delta;
						http_msg_move_end(&txn->rsp, delta);
					}
					txn->flags &= ~TX_SCK_MASK;
					txn->flags |= TX_SCK_DELETED;
					/* and go on with next cookie */
				}
				else if (srv && srv->cookie && (t->be->ck_opts & PR_CK_RW)) {
					/* replace bytes val_beg->val_end with the cookie name associated
					 * with this server since we know it.
					 */
					delta = buffer_replace2(res->buf, val_beg, val_end, srv->cookie, srv->cklen);
					next     += delta;
					hdr_end  += delta;
					hdr_next += delta;
					cur_hdr->len += delta;
					http_msg_move_end(&txn->rsp, delta);

					txn->flags &= ~TX_SCK_MASK;
					txn->flags |= TX_SCK_REPLACED;
				}
				else if (srv && srv->cookie && (t->be->ck_opts & PR_CK_PFX)) {
					/* insert the cookie name associated with this server
					 * before existing cookie, and insert a delimiter between them..
					 */
					delta = buffer_replace2(res->buf, val_beg, val_beg, srv->cookie, srv->cklen + 1);
					next     += delta;
					hdr_end  += delta;
					hdr_next += delta;
					cur_hdr->len += delta;
					http_msg_move_end(&txn->rsp, delta);

					val_beg[srv->cklen] = COOKIE_DELIM;
					txn->flags &= ~TX_SCK_MASK;
					txn->flags |= TX_SCK_REPLACED;
				}
			}
			/* next, let's see if the cookie is our appcookie, unless persistence must be ignored */
			else if (!(t->flags & SN_IGNORE_PRST) && (t->be->appsession_name != NULL)) {
				int cmp_len, value_len;
				char *value_begin;

				if (t->be->options2 & PR_O2_AS_PFX) {
					cmp_len = MIN(val_end - att_beg, t->be->appsession_name_len);
					value_begin = att_beg + t->be->appsession_name_len;
					value_len = MIN(t->be->appsession_len, val_end - att_beg - t->be->appsession_name_len);
				} else {
					cmp_len = att_end - att_beg;
					value_begin = val_beg;
					value_len = MIN(t->be->appsession_len, val_end - val_beg);
				}

				if ((cmp_len == t->be->appsession_name_len) &&
				    (memcmp(att_beg, t->be->appsession_name, t->be->appsession_name_len) == 0)) {
					/* free a possibly previously allocated memory */
					pool_free2(apools.sessid, txn->sessid);

					/* Store the sessid in the session for future use */
					if ((txn->sessid = pool_alloc2(apools.sessid)) == NULL) {
						Alert("Not enough Memory process_srv():asession->sessid:malloc().\n");
						send_log(t->be, LOG_ALERT, "Not enough Memory process_srv():asession->sessid:malloc().\n");
						return;
					}
					memcpy(txn->sessid, value_begin, value_len);
					txn->sessid[value_len] = 0;
				}
			}
			/* that's done for this cookie, check the next one on the same
			 * line when next != hdr_end (only if is_cookie2).
			 */
		}
		/* check next header */
		old_idx = cur_idx;
	}

	if (txn->sessid != NULL) {
		appsess *asession = NULL;
		/* only do insert, if lookup fails */
		asession = appsession_hash_lookup(&(t->be->htbl_proxy), txn->sessid);
		if (asession == NULL) {
			size_t server_id_len;
			if ((asession = pool_alloc2(pool2_appsess)) == NULL) {
				Alert("Not enough Memory process_srv():asession:calloc().\n");
				send_log(t->be, LOG_ALERT, "Not enough Memory process_srv():asession:calloc().\n");
				return;
			}
			asession->serverid = NULL; /* to avoid a double free in case of allocation error */

			if ((asession->sessid = pool_alloc2(apools.sessid)) == NULL) {
				Alert("Not enough Memory process_srv():asession->sessid:malloc().\n");
				send_log(t->be, LOG_ALERT, "Not enough Memory process_srv():asession->sessid:malloc().\n");
				t->be->htbl_proxy.destroy(asession);
				return;
			}
			memcpy(asession->sessid, txn->sessid, t->be->appsession_len);
			asession->sessid[t->be->appsession_len] = 0;

			server_id_len = strlen(objt_server(t->target)->id) + 1;
			if ((asession->serverid = pool_alloc2(apools.serverid)) == NULL) {
				Alert("Not enough Memory process_srv():asession->serverid:malloc().\n");
				send_log(t->be, LOG_ALERT, "Not enough Memory process_srv():asession->sessid:malloc().\n");
				t->be->htbl_proxy.destroy(asession);
				return;
			}
			asession->serverid[0] = '\0';
			memcpy(asession->serverid, objt_server(t->target)->id, server_id_len);

			asession->request_count = 0;
			appsession_hash_insert(&(t->be->htbl_proxy), asession);
		}

		asession->expire = tick_add_ifset(now_ms, t->be->timeout.appsession);
		asession->request_count++;
	}
}