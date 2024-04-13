int http_reply_message(struct stream *s, struct http_reply *reply)
{
	struct channel *res = &s->res;
	struct htx *htx = htx_from_buf(&res->buf);

	if (s->txn->status == -1)
		s->txn->status = reply->status;
	channel_htx_truncate(res, htx);

	if (http_reply_to_htx(s, htx, reply) == -1)
		goto fail;

	htx_to_buf(htx, &s->res.buf);
	if (!http_forward_proxy_resp(s, 1))
		goto fail;
	return 0;

  fail:
	channel_htx_truncate(res, htx);
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_PRXCOND;
	return -1;
}