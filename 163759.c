void http_capture_bad_message(struct error_snapshot *es, struct session *s,
                              struct http_msg *msg,
			      int state, struct proxy *other_end)
{
	struct channel *chn = msg->chn;
	int len1, len2;

	es->len = MIN(chn->buf->i, sizeof(es->buf));
	len1 = chn->buf->data + chn->buf->size - chn->buf->p;
	len1 = MIN(len1, es->len);
	len2 = es->len - len1; /* remaining data if buffer wraps */

	memcpy(es->buf, chn->buf->p, len1);
	if (len2)
		memcpy(es->buf + len1, chn->buf->data, len2);

	if (msg->err_pos >= 0)
		es->pos = msg->err_pos;
	else
		es->pos = msg->next;

	es->when = date; // user-visible date
	es->sid  = s->uniq_id;
	es->srv  = objt_server(s->target);
	es->oe   = other_end;
	es->src  = s->req->prod->conn->addr.from;
	es->state = state;
	es->ev_id = error_snapshot_id++;
	es->b_flags = chn->flags;
	es->s_flags = s->flags;
	es->t_flags = s->txn.flags;
	es->m_flags = msg->flags;
	es->b_out = chn->buf->o;
	es->b_wrap = chn->buf->data + chn->buf->size - chn->buf->p;
	es->b_tot = chn->total;
	es->m_clen = msg->chunk_len;
	es->m_blen = msg->body_len;
}