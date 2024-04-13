void http_end_txn_clean_session(struct session *s)
{
	/* FIXME: We need a more portable way of releasing a backend's and a
	 * server's connections. We need a safer way to reinitialize buffer
	 * flags. We also need a more accurate method for computing per-request
	 * data.
	 */
	http_silent_debug(__LINE__, s);

	s->req->cons->flags |= SI_FL_NOLINGER | SI_FL_NOHALF;
	si_shutr(s->req->cons);
	si_shutw(s->req->cons);

	http_silent_debug(__LINE__, s);

	if (s->flags & SN_BE_ASSIGNED) {
		s->be->beconn--;
		if (unlikely(s->srv_conn))
			sess_change_server(s, NULL);
	}

	s->logs.t_close = tv_ms_elapsed(&s->logs.tv_accept, &now);
	session_process_counters(s);
	session_stop_backend_counters(s);

	if (s->txn.status) {
		int n;

		n = s->txn.status / 100;
		if (n < 1 || n > 5)
			n = 0;

		if (s->fe->mode == PR_MODE_HTTP) {
			s->fe->fe_counters.p.http.rsp[n]++;
			if (s->comp_algo && (s->flags & SN_COMP_READY))
				s->fe->fe_counters.p.http.comp_rsp++;
		}
		if ((s->flags & SN_BE_ASSIGNED) &&
		    (s->be->mode == PR_MODE_HTTP)) {
			s->be->be_counters.p.http.rsp[n]++;
			s->be->be_counters.p.http.cum_req++;
			if (s->comp_algo && (s->flags & SN_COMP_READY))
				s->be->be_counters.p.http.comp_rsp++;
		}
	}

	/* don't count other requests' data */
	s->logs.bytes_in  -= s->req->buf->i;
	s->logs.bytes_out -= s->rep->buf->i;

	/* let's do a final log if we need it */
	if (!LIST_ISEMPTY(&s->fe->logformat) && s->logs.logwait &&
	    !(s->flags & SN_MONITOR) &&
	    (!(s->fe->options & PR_O_NULLNOLOG) || s->req->total)) {
		s->do_log(s);
	}

	s->logs.accept_date = date; /* user-visible date for logging */
	s->logs.tv_accept = now;  /* corrected date for internal use */
	tv_zero(&s->logs.tv_request);
	s->logs.t_queue = -1;
	s->logs.t_connect = -1;
	s->logs.t_data = -1;
	s->logs.t_close = 0;
	s->logs.prx_queue_size = 0;  /* we get the number of pending conns before us */
	s->logs.srv_queue_size = 0; /* we will get this number soon */

	s->logs.bytes_in = s->req->total = s->req->buf->i;
	s->logs.bytes_out = s->rep->total = s->rep->buf->i;

	if (s->pend_pos)
		pendconn_free(s->pend_pos);

	if (objt_server(s->target)) {
		if (s->flags & SN_CURR_SESS) {
			s->flags &= ~SN_CURR_SESS;
			objt_server(s->target)->cur_sess--;
		}
		if (may_dequeue_tasks(objt_server(s->target), s->be))
			process_srv_queue(objt_server(s->target));
	}

	s->target = NULL;

	s->req->cons->state     = s->req->cons->prev_state = SI_ST_INI;
	s->req->cons->conn->t.sock.fd = -1; /* just to help with debugging */
	s->req->cons->conn->flags = CO_FL_NONE;
	s->req->cons->conn->err_code = CO_ER_NONE;
	s->req->cons->err_type  = SI_ET_NONE;
	s->req->cons->conn_retries = 0;  /* used for logging too */
	s->req->cons->err_loc   = NULL;
	s->req->cons->exp       = TICK_ETERNITY;
	s->req->cons->flags     = SI_FL_NONE;
	s->req->flags &= ~(CF_SHUTW|CF_SHUTW_NOW|CF_AUTO_CONNECT|CF_WRITE_ERROR|CF_STREAMER|CF_STREAMER_FAST|CF_NEVER_WAIT);
	s->rep->flags &= ~(CF_SHUTR|CF_SHUTR_NOW|CF_READ_ATTACHED|CF_READ_ERROR|CF_READ_NOEXP|CF_STREAMER|CF_STREAMER_FAST|CF_WRITE_PARTIAL|CF_NEVER_WAIT);
	s->flags &= ~(SN_DIRECT|SN_ASSIGNED|SN_ADDR_SET|SN_BE_ASSIGNED|SN_FORCE_PRST|SN_IGNORE_PRST);
	s->flags &= ~(SN_CURR_SESS|SN_REDIRECTABLE);

	if (s->flags & SN_COMP_READY)
		s->comp_algo->end(&s->comp_ctx);
	s->comp_algo = NULL;
	s->flags &= ~SN_COMP_READY;

	s->txn.meth = 0;
	http_reset_txn(s);
	s->txn.flags |= TX_NOT_FIRST | TX_WAIT_NEXT_RQ;
	if (s->fe->options2 & PR_O2_INDEPSTR)
		s->req->cons->flags |= SI_FL_INDEP_STR;

	if (s->fe->options2 & PR_O2_NODELAY) {
		s->req->flags |= CF_NEVER_WAIT;
		s->rep->flags |= CF_NEVER_WAIT;
	}

	/* if the request buffer is not empty, it means we're
	 * about to process another request, so send pending
	 * data with MSG_MORE to merge TCP packets when possible.
	 * Just don't do this if the buffer is close to be full,
	 * because the request will wait for it to flush a little
	 * bit before proceeding.
	 */
	if (s->req->buf->i) {
		if (s->rep->buf->o &&
		    !buffer_full(s->rep->buf, global.tune.maxrewrite) &&
		    bi_end(s->rep->buf) <= s->rep->buf->data + s->rep->buf->size - global.tune.maxrewrite)
			s->rep->flags |= CF_EXPECT_MORE;
	}

	/* we're removing the analysers, we MUST re-enable events detection */
	channel_auto_read(s->req);
	channel_auto_close(s->req);
	channel_auto_read(s->rep);
	channel_auto_close(s->rep);

	s->req->analysers = s->listener->analysers;
	s->rep->analysers = 0;

	http_silent_debug(__LINE__, s);
}