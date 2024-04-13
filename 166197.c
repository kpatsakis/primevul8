struct http_reply *http_error_message(struct stream *s)
{
	const int msgnum = http_get_status_idx(s->txn->status);

	if (s->txn->http_reply)
		return s->txn->http_reply;
	else if (s->be->replies[msgnum])
		return s->be->replies[msgnum];
	else if (strm_fe(s)->replies[msgnum])
		return strm_fe(s)->replies[msgnum];
	else
		return &http_err_replies[msgnum];
}