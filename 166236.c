void http_reply_and_close(struct stream *s, short status, struct http_reply *msg)
{
	if (!msg) {
		channel_htx_truncate(&s->res, htxbuf(&s->res.buf));
		goto end;
	}

	if (http_reply_message(s, msg) == -1) {
		/* On error, return a 500 error message, but don't rewrite it if
		 * it is already an internal error. If it was already a "const"
		 * 500 error, just fail.
		 */
		if (s->txn->status == 500) {
			if (s->txn->flags & TX_CONST_REPLY)
				goto end;
			s->txn->flags |= TX_CONST_REPLY;
		}
		s->txn->status = 500;
		s->txn->http_reply = NULL;
		return http_reply_and_close(s, s->txn->status, http_error_message(s));
	}

end:
	s->res.wex = tick_add_ifset(now_ms, s->res.wto);

	/* At this staged, HTTP analysis is finished */
	s->req.analysers &= AN_REQ_FLT_END;
	s->req.analyse_exp = TICK_ETERNITY;

	s->res.analysers &= AN_RES_FLT_END;
	s->res.analyse_exp = TICK_ETERNITY;

	channel_auto_read(&s->req);
	channel_abort(&s->req);
	channel_auto_close(&s->req);
	channel_htx_erase(&s->req, htxbuf(&s->req.buf));
	channel_auto_read(&s->res);
	channel_auto_close(&s->res);
	channel_shutr_now(&s->res);
}