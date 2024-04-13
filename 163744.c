int http_process_res_common(struct session *t, struct channel *rep, int an_bit, struct proxy *px)
{
	struct http_txn *txn = &t->txn;
	struct http_msg *msg = &txn->rsp;
	struct proxy *cur_proxy;
	struct cond_wordlist *wl;

	DPRINTF(stderr,"[%u] %s: session=%p b=%p, exp(r,w)=%u,%u bf=%08x bh=%d analysers=%02x\n",
		now_ms, __FUNCTION__,
		t,
		rep,
		rep->rex, rep->wex,
		rep->flags,
		rep->buf->i,
		rep->analysers);

	if (unlikely(msg->msg_state < HTTP_MSG_BODY))	/* we need more data */
		return 0;

	rep->analysers &= ~an_bit;
	rep->analyse_exp = TICK_ETERNITY;

	/* Now we have to check if we need to modify the Connection header.
	 * This is more difficult on the response than it is on the request,
	 * because we can have two different HTTP versions and we don't know
	 * how the client will interprete a response. For instance, let's say
	 * that the client sends a keep-alive request in HTTP/1.0 and gets an
	 * HTTP/1.1 response without any header. Maybe it will bound itself to
	 * HTTP/1.0 because it only knows about it, and will consider the lack
	 * of header as a close, or maybe it knows HTTP/1.1 and can consider
	 * the lack of header as a keep-alive. Thus we will use two flags
	 * indicating how a request MAY be understood by the client. In case
	 * of multiple possibilities, we'll fix the header to be explicit. If
	 * ambiguous cases such as both close and keepalive are seen, then we
	 * will fall back to explicit close. Note that we won't take risks with
	 * HTTP/1.0 clients which may not necessarily understand keep-alive.
	 * See doc/internals/connection-header.txt for the complete matrix.
	 */

	if (unlikely((txn->meth == HTTP_METH_CONNECT && txn->status == 200) ||
		     txn->status == 101)) {
		/* Either we've established an explicit tunnel, or we're
		 * switching the protocol. In both cases, we're very unlikely
		 * to understand the next protocols. We have to switch to tunnel
		 * mode, so that we transfer the request and responses then let
		 * this protocol pass unmodified. When we later implement specific
		 * parsers for such protocols, we'll want to check the Upgrade
		 * header which contains information about that protocol for
		 * responses with status 101 (eg: see RFC2817 about TLS).
		 */
		txn->flags = (txn->flags & ~TX_CON_WANT_MSK) | TX_CON_WANT_TUN;
	}
	else if ((txn->status >= 200) && !(txn->flags & TX_HDR_CONN_PRS) &&
		 ((txn->flags & TX_CON_WANT_MSK) != TX_CON_WANT_TUN ||
		  ((t->fe->options|t->be->options) & PR_O_HTTP_CLOSE))) {
		int to_del = 0;

		/* on unknown transfer length, we must close */
		if (!(msg->flags & HTTP_MSGF_XFER_LEN) &&
		    (txn->flags & TX_CON_WANT_MSK) != TX_CON_WANT_TUN)
			txn->flags = (txn->flags & ~TX_CON_WANT_MSK) | TX_CON_WANT_CLO;

		/* now adjust header transformations depending on current state */
		if ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_TUN ||
		    (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_CLO) {
			to_del |= 2; /* remove "keep-alive" on any response */
			if (!(msg->flags & HTTP_MSGF_VER_11))
				to_del |= 1; /* remove "close" for HTTP/1.0 responses */
		}
		else { /* SCL / KAL */
			to_del |= 1; /* remove "close" on any response */
			if (txn->req.flags & msg->flags & HTTP_MSGF_VER_11)
				to_del |= 2; /* remove "keep-alive" on pure 1.1 responses */
		}

		/* Parse and remove some headers from the connection header */
		http_parse_connection_header(txn, msg, to_del);

		/* Some keep-alive responses are converted to Server-close if
		 * the server wants to close.
		 */
		if ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL) {
			if ((txn->flags & TX_HDR_CONN_CLO) ||
			    (!(txn->flags & TX_HDR_CONN_KAL) && !(msg->flags & HTTP_MSGF_VER_11)))
				txn->flags = (txn->flags & ~TX_CON_WANT_MSK) | TX_CON_WANT_SCL;
		}
	}

	if (1) {
		/*
		 * 3: we will have to evaluate the filters.
		 * As opposed to version 1.2, now they will be evaluated in the
		 * filters order and not in the header order. This means that
		 * each filter has to be validated among all headers.
		 *
		 * Filters are tried with ->be first, then with ->fe if it is
		 * different from ->be.
		 */

		cur_proxy = t->be;
		while (1) {
			struct proxy *rule_set = cur_proxy;

			/* try headers filters */
			if (rule_set->rsp_exp != NULL) {
				if (apply_filters_to_response(t, rep, rule_set) < 0) {
				return_bad_resp:
					if (objt_server(t->target)) {
						objt_server(t->target)->counters.failed_resp++;
						health_adjust(objt_server(t->target), HANA_STATUS_HTTP_RSP);
					}
					t->be->be_counters.failed_resp++;
				return_srv_prx_502:
					rep->analysers = 0;
					txn->status = 502;
					rep->prod->flags |= SI_FL_NOLINGER;
					bi_erase(rep);
					stream_int_retnclose(rep->cons, http_error_message(t, HTTP_ERR_502));
					if (!(t->flags & SN_ERR_MASK))
						t->flags |= SN_ERR_PRXCOND;
					if (!(t->flags & SN_FINST_MASK))
						t->flags |= SN_FINST_H;
					return 0;
				}
			}

			/* has the response been denied ? */
			if (txn->flags & TX_SVDENY) {
				if (objt_server(t->target))
					objt_server(t->target)->counters.failed_secu++;

				t->be->be_counters.denied_resp++;
				t->fe->fe_counters.denied_resp++;
				if (t->listener->counters)
					t->listener->counters->denied_resp++;

				goto return_srv_prx_502;
			}

			/* add response headers from the rule sets in the same order */
			list_for_each_entry(wl, &rule_set->rsp_add, list) {
				if (txn->status < 200)
					break;
				if (wl->cond) {
					int ret = acl_exec_cond(wl->cond, px, t, txn, SMP_OPT_DIR_RES|SMP_OPT_FINAL);
					ret = acl_pass(ret);
					if (((struct acl_cond *)wl->cond)->pol == ACL_COND_UNLESS)
						ret = !ret;
					if (!ret)
						continue;
				}
				if (unlikely(http_header_add_tail(&txn->rsp, &txn->hdr_idx, wl->s) < 0))
					goto return_bad_resp;
			}

			/* check whether we're already working on the frontend */
			if (cur_proxy == t->fe)
				break;
			cur_proxy = t->fe;
		}

		/*
		 * We may be facing a 100-continue response, in which case this
		 * is not the right response, and we're waiting for the next one.
		 * Let's allow this response to go to the client and wait for the
		 * next one.
		 */
		if (unlikely(txn->status == 100)) {
			hdr_idx_init(&txn->hdr_idx);
			msg->next -= channel_forward(rep, msg->next);
			msg->msg_state = HTTP_MSG_RPBEFORE;
			txn->status = 0;
			rep->analysers |= AN_RES_WAIT_HTTP | an_bit;
			return 1;
		}
		else if (unlikely(txn->status < 200))
			goto skip_header_mangling;

		/* we don't have any 1xx status code now */

		/*
		 * 4: check for server cookie.
		 */
		if (t->be->cookie_name || t->be->appsession_name || t->fe->capture_name ||
		    (t->be->options & PR_O_CHK_CACHE))
			manage_server_side_cookies(t, rep);


		/*
		 * 5: check for cache-control or pragma headers if required.
		 */
		if ((t->be->options & PR_O_CHK_CACHE) || (t->be->ck_opts & PR_CK_NOC))
			check_response_for_cacheability(t, rep);

		/*
		 * 6: add server cookie in the response if needed
		 */
		if (objt_server(t->target) && (t->be->ck_opts & PR_CK_INS) &&
		    !((txn->flags & TX_SCK_FOUND) && (t->be->ck_opts & PR_CK_PSV)) &&
		    (!(t->flags & SN_DIRECT) ||
		     ((t->be->cookie_maxidle || txn->cookie_last_date) &&
		      (!txn->cookie_last_date || (txn->cookie_last_date - date.tv_sec) < 0)) ||
		     (t->be->cookie_maxlife && !txn->cookie_first_date) ||  // set the first_date
		     (!t->be->cookie_maxlife && txn->cookie_first_date)) && // remove the first_date
		    (!(t->be->ck_opts & PR_CK_POST) || (txn->meth == HTTP_METH_POST)) &&
		    !(t->flags & SN_IGNORE_PRST)) {
			/* the server is known, it's not the one the client requested, or the
			 * cookie's last seen date needs to be refreshed. We have to
			 * insert a set-cookie here, except if we want to insert only on POST
			 * requests and this one isn't. Note that servers which don't have cookies
			 * (eg: some backup servers) will return a full cookie removal request.
			 */
			if (!objt_server(t->target)->cookie) {
				chunk_printf(&trash,
					      "Set-Cookie: %s=; Expires=Thu, 01-Jan-1970 00:00:01 GMT; path=/",
					      t->be->cookie_name);
			}
			else {
				chunk_printf(&trash, "Set-Cookie: %s=%s", t->be->cookie_name, objt_server(t->target)->cookie);

				if (t->be->cookie_maxidle || t->be->cookie_maxlife) {
					/* emit last_date, which is mandatory */
					trash.str[trash.len++] = COOKIE_DELIM_DATE;
					s30tob64((date.tv_sec+3) >> 2, trash.str + trash.len);
					trash.len += 5;

					if (t->be->cookie_maxlife) {
						/* emit first_date, which is either the original one or
						 * the current date.
						 */
						trash.str[trash.len++] = COOKIE_DELIM_DATE;
						s30tob64(txn->cookie_first_date ?
							 txn->cookie_first_date >> 2 :
							 (date.tv_sec+3) >> 2, trash.str + trash.len);
						trash.len += 5;
					}
				}
				chunk_appendf(&trash, "; path=/");
			}

			if (t->be->cookie_domain)
				chunk_appendf(&trash, "; domain=%s", t->be->cookie_domain);

			if (t->be->ck_opts & PR_CK_HTTPONLY)
				chunk_appendf(&trash, "; HttpOnly");

			if (t->be->ck_opts & PR_CK_SECURE)
				chunk_appendf(&trash, "; Secure");

			if (unlikely(http_header_add_tail2(&txn->rsp, &txn->hdr_idx, trash.str, trash.len) < 0))
				goto return_bad_resp;

			txn->flags &= ~TX_SCK_MASK;
			if (objt_server(t->target)->cookie && (t->flags & SN_DIRECT))
				/* the server did not change, only the date was updated */
				txn->flags |= TX_SCK_UPDATED;
			else
				txn->flags |= TX_SCK_INSERTED;

			/* Here, we will tell an eventual cache on the client side that we don't
			 * want it to cache this reply because HTTP/1.0 caches also cache cookies !
			 * Some caches understand the correct form: 'no-cache="set-cookie"', but
			 * others don't (eg: apache <= 1.3.26). So we use 'private' instead.
			 */
			if ((t->be->ck_opts & PR_CK_NOC) && (txn->flags & TX_CACHEABLE)) {

				txn->flags &= ~TX_CACHEABLE & ~TX_CACHE_COOK;

				if (unlikely(http_header_add_tail2(&txn->rsp, &txn->hdr_idx,
								   "Cache-control: private", 22) < 0))
					goto return_bad_resp;
			}
		}

		/*
		 * 7: check if result will be cacheable with a cookie.
		 * We'll block the response if security checks have caught
		 * nasty things such as a cacheable cookie.
		 */
		if (((txn->flags & (TX_CACHEABLE | TX_CACHE_COOK | TX_SCK_PRESENT)) ==
		     (TX_CACHEABLE | TX_CACHE_COOK | TX_SCK_PRESENT)) &&
		    (t->be->options & PR_O_CHK_CACHE)) {

			/* we're in presence of a cacheable response containing
			 * a set-cookie header. We'll block it as requested by
			 * the 'checkcache' option, and send an alert.
			 */
			if (objt_server(t->target))
				objt_server(t->target)->counters.failed_secu++;

			t->be->be_counters.denied_resp++;
			t->fe->fe_counters.denied_resp++;
			if (t->listener->counters)
				t->listener->counters->denied_resp++;

			Alert("Blocking cacheable cookie in response from instance %s, server %s.\n",
			      t->be->id, objt_server(t->target) ? objt_server(t->target)->id : "<dispatch>");
			send_log(t->be, LOG_ALERT,
				 "Blocking cacheable cookie in response from instance %s, server %s.\n",
				 t->be->id, objt_server(t->target) ? objt_server(t->target)->id : "<dispatch>");
			goto return_srv_prx_502;
		}

		/*
		 * 8: adjust "Connection: close" or "Connection: keep-alive" if needed.
		 * If an "Upgrade" token is found, the header is left untouched in order
		 * not to have to deal with some client bugs : some of them fail an upgrade
		 * if anything but "Upgrade" is present in the Connection header.
		 */
		if (!(txn->flags & TX_HDR_CONN_UPG) &&
		    (((txn->flags & TX_CON_WANT_MSK) != TX_CON_WANT_TUN) ||
		     ((t->fe->options|t->be->options) & PR_O_HTTP_CLOSE))) {
			unsigned int want_flags = 0;

			if ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL ||
			    (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL) {
				/* we want a keep-alive response here. Keep-alive header
				 * required if either side is not 1.1.
				 */
				if (!(txn->req.flags & msg->flags & HTTP_MSGF_VER_11))
					want_flags |= TX_CON_KAL_SET;
			}
			else {
				/* we want a close response here. Close header required if
				 * the server is 1.1, regardless of the client.
				 */
				if (msg->flags & HTTP_MSGF_VER_11)
					want_flags |= TX_CON_CLO_SET;
			}

			if (want_flags != (txn->flags & (TX_CON_CLO_SET|TX_CON_KAL_SET)))
				http_change_connection_header(txn, msg, want_flags);
		}

	skip_header_mangling:
		if ((msg->flags & HTTP_MSGF_XFER_LEN) ||
		    (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_TUN)
			rep->analysers |= AN_RES_HTTP_XFER_BODY;

		/*************************************************************
		 * OK, that's finished for the headers. We have done what we *
		 * could. Let's switch to the DATA state.                    *
		 ************************************************************/

		t->logs.t_data = tv_ms_elapsed(&t->logs.tv_accept, &now);

		/* if the user wants to log as soon as possible, without counting
		 * bytes from the server, then this is the right moment. We have
		 * to temporarily assign bytes_out to log what we currently have.
		 */
		if (!LIST_ISEMPTY(&t->fe->logformat) && !(t->logs.logwait & LW_BYTES)) {
			t->logs.t_close = t->logs.t_data; /* to get a valid end date */
			t->logs.bytes_out = txn->rsp.eoh;
			t->do_log(t);
			t->logs.bytes_out = 0;
		}

		/* Note: we must not try to cheat by jumping directly to DATA,
		 * otherwise we would not let the client side wake up.
		 */

		return 1;
	}
	return 1;
}