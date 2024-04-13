static void http_silent_debug(int line, struct session *s)
{
	chunk_printf(&trash,
	             "[%04d] req: p=%d(%d) s=%d bf=%08x an=%08x data=%p size=%d l=%d w=%p r=%p o=%p sm=%d fw=%ld tf=%08x\n",
	             line,
	             s->si[0].state, s->si[0].fd, s->txn.req.msg_state, s->req->flags, s->req->analysers,
	             s->req->buf->data, s->req->buf->size, s->req->l, s->req->w, s->req->r, s->req->buf->p, s->req->buf->o, s->req->to_forward, s->txn.flags);
	write(-1, trash.str, trash.len);

	chunk_printf(&trash,
	             " %04d  rep: p=%d(%d) s=%d bf=%08x an=%08x data=%p size=%d l=%d w=%p r=%p o=%p sm=%d fw=%ld\n",
                     line,
	             s->si[1].state, s->si[1].fd, s->txn.rsp.msg_state, s->rep->flags, s->rep->analysers,
	             s->rep->buf->data, s->rep->buf->size, s->rep->l, s->rep->w, s->rep->r, s->rep->buf->p, s->rep->buf->o, s->rep->to_forward);
	write(-1, trash.str, trash.len);
}