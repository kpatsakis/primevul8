int http_reply_to_htx(struct stream *s, struct htx *htx, struct http_reply *reply)
{
	struct buffer *errmsg;
	struct htx_sl *sl;
	struct buffer *body = NULL;
	const char *status, *reason, *clen, *ctype;
	unsigned int slflags;
	int ret = 0;

	/*
	 * - HTTP_REPLY_ERRFILES unexpected here. handled as no payload if so
	 *
	 * - HTTP_REPLY_INDIRECT: switch on another reply if defined or handled
	 *   as no payload if NULL. the TXN status code is set with the status
	 *   of the original reply.
	 */

	if (reply->type == HTTP_REPLY_INDIRECT) {
		if (reply->body.reply)
			reply = reply->body.reply;
	}
	if (reply->type == HTTP_REPLY_ERRMSG && !reply->body.errmsg)  {
		/* get default error message */
		if (reply == s->txn->http_reply)
			s->txn->http_reply = NULL;
		reply = http_error_message(s);
		if (reply->type == HTTP_REPLY_INDIRECT) {
			if (reply->body.reply)
				reply = reply->body.reply;
		}
	}

	if (reply->type == HTTP_REPLY_ERRMSG) {
		/* implicit or explicit error message*/
		errmsg = reply->body.errmsg;
		if (errmsg && !b_is_null(errmsg)) {
			if (!htx_copy_msg(htx, errmsg))
				goto fail;
		}
	}
	else {
		/* no payload, file or log-format string */
		if (reply->type == HTTP_REPLY_RAW) {
			/* file */
			body = &reply->body.obj;
		}
		else if (reply->type == HTTP_REPLY_LOGFMT) {
			/* log-format string */
			body = alloc_trash_chunk();
			if (!body)
				goto fail_alloc;
			body->data = build_logline(s, body->area, body->size, &reply->body.fmt);
		}
		/* else no payload */

		status = ultoa(reply->status);
		reason = http_get_reason(reply->status);
		slflags = (HTX_SL_F_IS_RESP|HTX_SL_F_VER_11|HTX_SL_F_XFER_LEN|HTX_SL_F_CLEN);
		if (!body || !b_data(body))
			slflags |= HTX_SL_F_BODYLESS;
		sl = htx_add_stline(htx, HTX_BLK_RES_SL, slflags, ist("HTTP/1.1"), ist(status), ist(reason));
		if (!sl)
			goto fail;
		sl->info.res.status = reply->status;

		clen = (body ? ultoa(b_data(body)) : "0");
		ctype = reply->ctype;

		if (!LIST_ISEMPTY(&reply->hdrs)) {
			struct http_reply_hdr *hdr;
			struct buffer *value = alloc_trash_chunk();

			if (!value)
				goto fail;

			list_for_each_entry(hdr, &reply->hdrs, list) {
				chunk_reset(value);
				value->data = build_logline(s, value->area, value->size, &hdr->value);
				if (b_data(value) && !htx_add_header(htx, hdr->name, ist2(b_head(value), b_data(value)))) {
					free_trash_chunk(value);
					goto fail;
				}
				chunk_reset(value);
			}
			free_trash_chunk(value);
		}

		if (!htx_add_header(htx, ist("content-length"), ist(clen)) ||
		    (body && b_data(body) && ctype && !htx_add_header(htx, ist("content-type"), ist(ctype))) ||
		    !htx_add_endof(htx, HTX_BLK_EOH) ||
		    (body && b_data(body) && !htx_add_data_atonce(htx, ist2(b_head(body), b_data(body)))))
			goto fail;

		htx->flags |= HTX_FL_EOM;
	}

  leave:
	if (reply->type == HTTP_REPLY_LOGFMT)
		free_trash_chunk(body);
	return ret;

  fail_alloc:
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_RESOURCE;
	/* fall through */
  fail:
	ret = -1;
	goto leave;
}