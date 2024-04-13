int http_forward_proxy_resp(struct stream *s, int final)
{
	struct channel *req = &s->req;
	struct channel *res = &s->res;
	struct htx *htx = htxbuf(&res->buf);
	size_t data;

	if (final) {
		htx->flags |= HTX_FL_PROXY_RESP;

		if (!htx_is_empty(htx) && !http_eval_after_res_rules(s))
			return 0;

		if (s->txn->meth == HTTP_METH_HEAD)
			htx_skip_msg_payload(htx);

		channel_auto_read(req);
		channel_abort(req);
		channel_auto_close(req);
		channel_htx_erase(req, htxbuf(&req->buf));

		res->wex = tick_add_ifset(now_ms, res->wto);
		channel_auto_read(res);
		channel_auto_close(res);
		channel_shutr_now(res);
		res->flags |= CF_EOI; /* The response is terminated, add EOI */
		htxbuf(&res->buf)->flags |= HTX_FL_EOM; /* no more data are expected */
	}
	else {
		/* Send ASAP informational messages. Rely on CF_EOI for final
		 * response.
		 */
		res->flags |= CF_SEND_DONTWAIT;
	}

	data = htx->data - co_data(res);
	c_adv(res, data);
	htx->first = -1;
	res->total += data;
	return 1;
}