static __inline int do_l7_retry(struct stream *s, struct stream_interface *si)
{
	struct channel *req, *res;
	int co_data;

	si->conn_retries--;
	if (si->conn_retries < 0)
		return -1;

	if (objt_server(s->target)) {
		if (s->flags & SF_CURR_SESS) {
			s->flags &= ~SF_CURR_SESS;
			_HA_ATOMIC_DEC(&__objt_server(s->target)->cur_sess);
		}
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.retries);
	}
	_HA_ATOMIC_INC(&s->be->be_counters.retries);

	req = &s->req;
	res = &s->res;
	/* Remove any write error from the request, and read error from the response */
	req->flags &= ~(CF_WRITE_ERROR | CF_WRITE_TIMEOUT | CF_SHUTW | CF_SHUTW_NOW);
	res->flags &= ~(CF_READ_ERROR | CF_READ_TIMEOUT | CF_SHUTR | CF_EOI | CF_READ_NULL | CF_SHUTR_NOW);
	res->analysers &= AN_RES_FLT_END;
	si->flags &= ~(SI_FL_ERR | SI_FL_EXP | SI_FL_RXBLK_SHUT);
	si->err_type = SI_ET_NONE;
	s->flags &= ~(SF_ERR_MASK | SF_FINST_MASK);
	stream_choose_redispatch(s);
	si->exp = TICK_ETERNITY;
	res->rex = TICK_ETERNITY;
	res->to_forward = 0;
	res->analyse_exp = TICK_ETERNITY;
	res->total = 0;
	si_release_endpoint(&s->si[1]);

	b_free(&req->buf);
	/* Swap the L7 buffer with the channel buffer */
	/* We know we stored the co_data as b_data, so get it there */
	co_data = b_data(&si->l7_buffer);
	b_set_data(&si->l7_buffer, b_size(&si->l7_buffer));
	b_xfer(&req->buf, &si->l7_buffer, b_data(&si->l7_buffer));
	co_set_data(req, co_data);

	DBG_TRACE_DEVEL("perform a L7 retry", STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, s->txn);

	b_reset(&res->buf);
	co_set_data(res, 0);
	return 0;
}