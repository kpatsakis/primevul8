static int http_reply_100_continue(struct stream *s)
{
	struct channel *res = &s->res;
	struct htx *htx = htx_from_buf(&res->buf);
	struct htx_sl *sl;
	unsigned int flags = (HTX_SL_F_IS_RESP|HTX_SL_F_VER_11|
			      HTX_SL_F_XFER_LEN|HTX_SL_F_BODYLESS);

	sl = htx_add_stline(htx, HTX_BLK_RES_SL, flags,
			    ist("HTTP/1.1"), ist("100"), ist("Continue"));
	if (!sl)
		goto fail;
	sl->info.res.status = 100;

	if (!htx_add_endof(htx, HTX_BLK_EOH))
		goto fail;

	if (!http_forward_proxy_resp(s, 0))
		goto fail;
	return 0;

  fail:
	/* If an error occurred, remove the incomplete HTTP response from the
	 * buffer */
	channel_htx_truncate(res, htx);
	return -1;
}