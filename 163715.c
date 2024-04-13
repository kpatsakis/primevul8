static int http_upgrade_v09_to_v10(struct http_txn *txn)
{
	int delta;
	char *cur_end;
	struct http_msg *msg = &txn->req;

	if (msg->sl.rq.v_l != 0)
		return 1;

	cur_end = msg->chn->buf->p + msg->sl.rq.l;
	delta = 0;

	if (msg->sl.rq.u_l == 0) {
		/* if no URI was set, add "/" */
		delta = buffer_replace2(msg->chn->buf, cur_end, cur_end, " /", 2);
		cur_end += delta;
		http_msg_move_end(msg, delta);
	}
	/* add HTTP version */
	delta = buffer_replace2(msg->chn->buf, cur_end, cur_end, " HTTP/1.0\r\n", 11);
	http_msg_move_end(msg, delta);
	cur_end += delta;
	cur_end = (char *)http_parse_reqline(msg,
					     HTTP_MSG_RQMETH,
					     msg->chn->buf->p, cur_end + 1,
					     NULL, NULL);
	if (unlikely(!cur_end))
		return 0;

	/* we have a full HTTP/1.0 request now and we know that
	 * we have either a CR or an LF at <ptr>.
	 */
	hdr_idx_set_start(&txn->hdr_idx, msg->sl.rq.l, *cur_end == '\r');
	return 1;
}