void http_server_error(struct stream *s, struct stream_interface *si, int err,
		       int finst, struct http_reply *msg)
{
	http_reply_and_close(s, s->txn->status, msg);
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= err;
	if (!(s->flags & SF_FINST_MASK))
		s->flags |= finst;
}