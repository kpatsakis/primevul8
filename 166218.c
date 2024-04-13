static int http_handle_expect_hdr(struct stream *s, struct htx *htx, struct http_msg *msg)
{
	/* If we have HTTP/1.1 message with a body and Expect: 100-continue,
	 * then we must send an HTTP/1.1 100 Continue intermediate response.
	 */
	if (msg->msg_state == HTTP_MSG_BODY && (msg->flags & HTTP_MSGF_VER_11) &&
	    (msg->flags & (HTTP_MSGF_CNT_LEN|HTTP_MSGF_TE_CHNK))) {
		struct ist hdr = { .ptr = "Expect", .len = 6 };
		struct http_hdr_ctx ctx;

		ctx.blk = NULL;
		/* Expect is allowed in 1.1, look for it */
		if (http_find_header(htx, hdr, &ctx, 0) &&
		    unlikely(isteqi(ctx.value, ist2("100-continue", 12)))) {
			if (http_reply_100_continue(s) == -1)
				return -1;
			http_remove_header(htx, &ctx);
		}
	}
	return 0;
}