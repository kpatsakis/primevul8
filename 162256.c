static void tcp_process_loss(struct sock *sk, int flag, bool is_dupack,
			     int *rexmit)
{
	struct tcp_sock *tp = tcp_sk(sk);
	bool recovered = !before(tp->snd_una, tp->high_seq);

	if ((flag & FLAG_SND_UNA_ADVANCED) &&
	    tcp_try_undo_loss(sk, false))
		return;

	if (tp->frto) { /* F-RTO RFC5682 sec 3.1 (sack enhanced version). */
		/* Step 3.b. A timeout is spurious if not all data are
		 * lost, i.e., never-retransmitted data are (s)acked.
		 */
		if ((flag & FLAG_ORIG_SACK_ACKED) &&
		    tcp_try_undo_loss(sk, true))
			return;

		if (after(tp->snd_nxt, tp->high_seq)) {
			if (flag & FLAG_DATA_SACKED || is_dupack)
				tp->frto = 0; /* Step 3.a. loss was real */
		} else if (flag & FLAG_SND_UNA_ADVANCED && !recovered) {
			tp->high_seq = tp->snd_nxt;
			/* Step 2.b. Try send new data (but deferred until cwnd
			 * is updated in tcp_ack()). Otherwise fall back to
			 * the conventional recovery.
			 */
			if (tcp_send_head(sk) &&
			    after(tcp_wnd_end(tp), tp->snd_nxt)) {
				*rexmit = REXMIT_NEW;
				return;
			}
			tp->frto = 0;
		}
	}

	if (recovered) {
		/* F-RTO RFC5682 sec 3.1 step 2.a and 1st part of step 3.a */
		tcp_try_undo_recovery(sk);
		return;
	}
	if (tcp_is_reno(tp)) {
		/* A Reno DUPACK means new data in F-RTO step 2.b above are
		 * delivered. Lower inflight to clock out (re)tranmissions.
		 */
		if (after(tp->snd_nxt, tp->high_seq) && is_dupack)
			tcp_add_reno_sack(sk);
		else if (flag & FLAG_SND_UNA_ADVANCED)
			tcp_reset_reno_sack(tp);
	}
	*rexmit = REXMIT_LOST;
}