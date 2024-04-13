int http_process_request(struct session *s, struct channel *req, int an_bit)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &txn->req;

	if (unlikely(msg->msg_state < HTTP_MSG_BODY)) {
		/* we need more data */
		channel_dont_connect(req);
		return 0;
	}

	DPRINTF(stderr,"[%u] %s: session=%p b=%p, exp(r,w)=%u,%u bf=%08x bh=%d analysers=%02x\n",
		now_ms, __FUNCTION__,
		s,
		req,
		req->rex, req->wex,
		req->flags,
		req->buf->i,
		req->analysers);

	if (s->fe->comp || s->be->comp)
		select_compression_request_header(s, req->buf);

	/*
	 * Right now, we know that we have processed the entire headers
	 * and that unwanted requests have been filtered out. We can do
	 * whatever we want with the remaining request. Also, now we
	 * may have separate values for ->fe, ->be.
	 */

	/*
	 * If HTTP PROXY is set we simply get remote server address
	 * parsing incoming request.
	 */
	if ((s->be->options & PR_O_HTTP_PROXY) && !(s->flags & SN_ADDR_SET)) {
		url2sa(req->buf->p + msg->sl.rq.u, msg->sl.rq.u_l, &s->req->cons->conn->addr.to);
	}

	/*
	 * 7: Now we can work with the cookies.
	 * Note that doing so might move headers in the request, but
	 * the fields will stay coherent and the URI will not move.
	 * This should only be performed in the backend.
	 */
	if ((s->be->cookie_name || s->be->appsession_name || s->fe->capture_name)
	    && !(txn->flags & (TX_CLDENY|TX_CLTARPIT)))
		manage_client_side_cookies(s, req);

	/*
	 * 8: the appsession cookie was looked up very early in 1.2,
	 * so let's do the same now.
	 */

	/* It needs to look into the URI unless persistence must be ignored */
	if ((txn->sessid == NULL) && s->be->appsession_name && !(s->flags & SN_IGNORE_PRST)) {
		get_srv_from_appsession(s, req->buf->p + msg->sl.rq.u, msg->sl.rq.u_l);
	}

	/* add unique-id if "header-unique-id" is specified */

	if (!LIST_ISEMPTY(&s->fe->format_unique_id))
		build_logline(s, s->unique_id, UNIQUEID_LEN, &s->fe->format_unique_id);

	if (s->fe->header_unique_id && s->unique_id) {
		chunk_printf(&trash, "%s: %s", s->fe->header_unique_id, s->unique_id);
		if (trash.len < 0)
			goto return_bad_req;
		if (unlikely(http_header_add_tail2(&txn->req, &txn->hdr_idx, trash.str, trash.len) < 0))
		   goto return_bad_req;
	}

	/*
	 * 9: add X-Forwarded-For if either the frontend or the backend
	 * asks for it.
	 */
	if ((s->fe->options | s->be->options) & PR_O_FWDFOR) {
		struct hdr_ctx ctx = { .idx = 0 };
		if (!((s->fe->options | s->be->options) & PR_O_FF_ALWAYS) &&
			http_find_header2(s->be->fwdfor_hdr_len ? s->be->fwdfor_hdr_name : s->fe->fwdfor_hdr_name,
			                  s->be->fwdfor_hdr_len ? s->be->fwdfor_hdr_len : s->fe->fwdfor_hdr_len,
			                  req->buf->p, &txn->hdr_idx, &ctx)) {
			/* The header is set to be added only if none is present
			 * and we found it, so don't do anything.
			 */
		}
		else if (s->req->prod->conn->addr.from.ss_family == AF_INET) {
			/* Add an X-Forwarded-For header unless the source IP is
			 * in the 'except' network range.
			 */
			if ((!s->fe->except_mask.s_addr ||
			     (((struct sockaddr_in *)&s->req->prod->conn->addr.from)->sin_addr.s_addr & s->fe->except_mask.s_addr)
			     != s->fe->except_net.s_addr) &&
			    (!s->be->except_mask.s_addr ||
			     (((struct sockaddr_in *)&s->req->prod->conn->addr.from)->sin_addr.s_addr & s->be->except_mask.s_addr)
			     != s->be->except_net.s_addr)) {
				int len;
				unsigned char *pn;
				pn = (unsigned char *)&((struct sockaddr_in *)&s->req->prod->conn->addr.from)->sin_addr;

				/* Note: we rely on the backend to get the header name to be used for
				 * x-forwarded-for, because the header is really meant for the backends.
				 * However, if the backend did not specify any option, we have to rely
				 * on the frontend's header name.
				 */
				if (s->be->fwdfor_hdr_len) {
					len = s->be->fwdfor_hdr_len;
					memcpy(trash.str, s->be->fwdfor_hdr_name, len);
				} else {
					len = s->fe->fwdfor_hdr_len;
					memcpy(trash.str, s->fe->fwdfor_hdr_name, len);
				}
				len += sprintf(trash.str + len, ": %d.%d.%d.%d", pn[0], pn[1], pn[2], pn[3]);

				if (unlikely(http_header_add_tail2(&txn->req, &txn->hdr_idx, trash.str, len) < 0))
					goto return_bad_req;
			}
		}
		else if (s->req->prod->conn->addr.from.ss_family == AF_INET6) {
			/* FIXME: for the sake of completeness, we should also support
			 * 'except' here, although it is mostly useless in this case.
			 */
			int len;
			char pn[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6,
				  (const void *)&((struct sockaddr_in6 *)(&s->req->prod->conn->addr.from))->sin6_addr,
				  pn, sizeof(pn));

			/* Note: we rely on the backend to get the header name to be used for
			 * x-forwarded-for, because the header is really meant for the backends.
			 * However, if the backend did not specify any option, we have to rely
			 * on the frontend's header name.
			 */
			if (s->be->fwdfor_hdr_len) {
				len = s->be->fwdfor_hdr_len;
				memcpy(trash.str, s->be->fwdfor_hdr_name, len);
			} else {
				len = s->fe->fwdfor_hdr_len;
				memcpy(trash.str, s->fe->fwdfor_hdr_name, len);
			}
			len += sprintf(trash.str + len, ": %s", pn);

			if (unlikely(http_header_add_tail2(&txn->req, &txn->hdr_idx, trash.str, len) < 0))
				goto return_bad_req;
		}
	}

	/*
	 * 10: add X-Original-To if either the frontend or the backend
	 * asks for it.
	 */
	if ((s->fe->options | s->be->options) & PR_O_ORGTO) {

		/* FIXME: don't know if IPv6 can handle that case too. */
		if (s->req->prod->conn->addr.from.ss_family == AF_INET) {
			/* Add an X-Original-To header unless the destination IP is
			 * in the 'except' network range.
			 */
			conn_get_to_addr(s->req->prod->conn);

			if (s->req->prod->conn->addr.to.ss_family == AF_INET &&
			    ((!s->fe->except_mask_to.s_addr ||
			      (((struct sockaddr_in *)&s->req->prod->conn->addr.to)->sin_addr.s_addr & s->fe->except_mask_to.s_addr)
			      != s->fe->except_to.s_addr) &&
			     (!s->be->except_mask_to.s_addr ||
			      (((struct sockaddr_in *)&s->req->prod->conn->addr.to)->sin_addr.s_addr & s->be->except_mask_to.s_addr)
			      != s->be->except_to.s_addr))) {
				int len;
				unsigned char *pn;
				pn = (unsigned char *)&((struct sockaddr_in *)&s->req->prod->conn->addr.to)->sin_addr;

				/* Note: we rely on the backend to get the header name to be used for
				 * x-original-to, because the header is really meant for the backends.
				 * However, if the backend did not specify any option, we have to rely
				 * on the frontend's header name.
				 */
				if (s->be->orgto_hdr_len) {
					len = s->be->orgto_hdr_len;
					memcpy(trash.str, s->be->orgto_hdr_name, len);
				} else {
					len = s->fe->orgto_hdr_len;
					memcpy(trash.str, s->fe->orgto_hdr_name, len);
				}
				len += sprintf(trash.str + len, ": %d.%d.%d.%d", pn[0], pn[1], pn[2], pn[3]);

				if (unlikely(http_header_add_tail2(&txn->req, &txn->hdr_idx, trash.str, len) < 0))
					goto return_bad_req;
			}
		}
	}

	/* 11: add "Connection: close" or "Connection: keep-alive" if needed and not yet set.
	 * If an "Upgrade" token is found, the header is left untouched in order not to have
	 * to deal with some servers bugs : some of them fail an Upgrade if anything but
	 * "Upgrade" is present in the Connection header.
	 */
	if (!(txn->flags & TX_HDR_CONN_UPG) &&
	    (((txn->flags & TX_CON_WANT_MSK) != TX_CON_WANT_TUN) ||
	     ((s->fe->options|s->be->options) & PR_O_HTTP_CLOSE))) {
		unsigned int want_flags = 0;

		if (msg->flags & HTTP_MSGF_VER_11) {
			if (((txn->flags & TX_CON_WANT_MSK) >= TX_CON_WANT_SCL ||
			    ((s->fe->options|s->be->options) & PR_O_HTTP_CLOSE)) &&
			    !((s->fe->options2|s->be->options2) & PR_O2_FAKE_KA))
				want_flags |= TX_CON_CLO_SET;
		} else {
			if (((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL &&
			     !((s->fe->options|s->be->options) & PR_O_HTTP_CLOSE)) ||
			    ((s->fe->options2|s->be->options2) & PR_O2_FAKE_KA))
				want_flags |= TX_CON_KAL_SET;
		}

		if (want_flags != (txn->flags & (TX_CON_CLO_SET|TX_CON_KAL_SET)))
			http_change_connection_header(txn, msg, want_flags);
	}


	/* If we have no server assigned yet and we're balancing on url_param
	 * with a POST request, we may be interested in checking the body for
	 * that parameter. This will be done in another analyser.
	 */
	if (!(s->flags & (SN_ASSIGNED|SN_DIRECT)) &&
	    s->txn.meth == HTTP_METH_POST && s->be->url_param_name != NULL &&
	    s->be->url_param_post_limit != 0 &&
	    (msg->flags & (HTTP_MSGF_CNT_LEN|HTTP_MSGF_TE_CHNK))) {
		channel_dont_connect(req);
		req->analysers |= AN_REQ_HTTP_BODY;
	}

	if (msg->flags & HTTP_MSGF_XFER_LEN) {
		req->analysers |= AN_REQ_HTTP_XFER_BODY;
#ifdef TCP_QUICKACK
		/* We expect some data from the client. Unless we know for sure
		 * we already have a full request, we have to re-enable quick-ack
		 * in case we previously disabled it, otherwise we might cause
		 * the client to delay further data.
		 */
		if ((s->listener->options & LI_O_NOQUICKACK) &&
		    ((msg->flags & HTTP_MSGF_TE_CHNK) ||
		     (msg->body_len > req->buf->i - txn->req.eoh - 2)))
			setsockopt(s->si[0].conn->t.sock.fd, IPPROTO_TCP, TCP_QUICKACK, &one, sizeof(one));
#endif
	}

	/*************************************************************
	 * OK, that's finished for the headers. We have done what we *
	 * could. Let's switch to the DATA state.                    *
	 ************************************************************/
	req->analyse_exp = TICK_ETERNITY;
	req->analysers &= ~an_bit;

	/* if the server closes the connection, we want to immediately react
	 * and close the socket to save packets and syscalls.
	 */
	if (!(req->analysers & AN_REQ_HTTP_XFER_BODY))
		req->cons->flags |= SI_FL_NOHALF;

	s->logs.tv_request = now;
	/* OK let's go on with the BODY now */
	return 1;

 return_bad_req: /* let's centralize all bad requests */
	if (unlikely(msg->msg_state == HTTP_MSG_ERROR) || msg->err_pos >= 0) {
		/* we detected a parsing error. We want to archive this request
		 * in the dedicated proxy area for later troubleshooting.
		 */
		http_capture_bad_message(&s->fe->invalid_req, s, msg, msg->msg_state, s->fe);
	}

	txn->req.msg_state = HTTP_MSG_ERROR;
	txn->status = 400;
	req->analysers = 0;
	stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_400));

	s->fe->fe_counters.failed_req++;
	if (s->listener->counters)
		s->listener->counters->failed_req++;

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_R;
	return 0;
}