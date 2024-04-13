void http_reset_txn(struct session *s)
{
	http_end_txn(s);
	http_init_txn(s);

	s->be = s->fe;
	s->logs.logwait = s->fe->to_log;
	session_del_srv_conn(s);
	s->target = NULL;
	/* re-init store persistence */
	s->store_count = 0;

	s->pend_pos = NULL;

	s->req->flags |= CF_READ_DONTWAIT; /* one read is usually enough */

	/* We must trim any excess data from the response buffer, because we
	 * may have blocked an invalid response from a server that we don't
	 * want to accidentely forward once we disable the analysers, nor do
	 * we want those data to come along with next response. A typical
	 * example of such data would be from a buggy server responding to
	 * a HEAD with some data, or sending more than the advertised
	 * content-length.
	 */
	if (unlikely(s->rep->buf->i))
		s->rep->buf->i = 0;

	s->req->rto = s->fe->timeout.client;
	s->req->wto = TICK_ETERNITY;

	s->rep->rto = TICK_ETERNITY;
	s->rep->wto = s->fe->timeout.client;

	s->req->rex = TICK_ETERNITY;
	s->req->wex = TICK_ETERNITY;
	s->req->analyse_exp = TICK_ETERNITY;
	s->rep->rex = TICK_ETERNITY;
	s->rep->wex = TICK_ETERNITY;
	s->rep->analyse_exp = TICK_ETERNITY;
}