tcp_timewait_state_process(struct inet_timewait_sock *tw, struct sk_buff *skb,
			   const struct tcphdr *th)
{
	struct tcp_options_received tmp_opt;
	struct tcp_timewait_sock *tcptw = tcp_twsk((struct sock *)tw);
	bool paws_reject = false;

	tmp_opt.saw_tstamp = 0;
	if (th->doff > (sizeof(*th) >> 2) && tcptw->tw_ts_recent_stamp) {
		tcp_parse_options(skb, &tmp_opt, 0, NULL);

		if (tmp_opt.saw_tstamp) {
			if (tmp_opt.rcv_tsecr)
				tmp_opt.rcv_tsecr -= tcptw->tw_ts_offset;
			tmp_opt.ts_recent	= tcptw->tw_ts_recent;
			tmp_opt.ts_recent_stamp	= tcptw->tw_ts_recent_stamp;
			paws_reject = tcp_paws_reject(&tmp_opt, th->rst);
		}
	}

	if (tw->tw_substate == TCP_FIN_WAIT2) {
		/* Just repeat all the checks of tcp_rcv_state_process() */

		/* Out of window, send ACK */
		if (paws_reject ||
		    !tcp_in_window(TCP_SKB_CB(skb)->seq, TCP_SKB_CB(skb)->end_seq,
				   tcptw->tw_rcv_nxt,
				   tcptw->tw_rcv_nxt + tcptw->tw_rcv_wnd))
			return tcp_timewait_check_oow_rate_limit(
				tw, skb, LINUX_MIB_TCPACKSKIPPEDFINWAIT2);

		if (th->rst)
			goto kill;

		if (th->syn && !before(TCP_SKB_CB(skb)->seq, tcptw->tw_rcv_nxt))
			return TCP_TW_RST;

		/* Dup ACK? */
		if (!th->ack ||
		    !after(TCP_SKB_CB(skb)->end_seq, tcptw->tw_rcv_nxt) ||
		    TCP_SKB_CB(skb)->end_seq == TCP_SKB_CB(skb)->seq) {
			inet_twsk_put(tw);
			return TCP_TW_SUCCESS;
		}

		/* New data or FIN. If new data arrive after half-duplex close,
		 * reset.
		 */
		if (!th->fin ||
		    TCP_SKB_CB(skb)->end_seq != tcptw->tw_rcv_nxt + 1)
			return TCP_TW_RST;

		/* FIN arrived, enter true time-wait state. */
		tw->tw_substate	  = TCP_TIME_WAIT;
		tcptw->tw_rcv_nxt = TCP_SKB_CB(skb)->end_seq;
		if (tmp_opt.saw_tstamp) {
			tcptw->tw_ts_recent_stamp = get_seconds();
			tcptw->tw_ts_recent	  = tmp_opt.rcv_tsval;
		}

		inet_twsk_reschedule(tw, TCP_TIMEWAIT_LEN);
		return TCP_TW_ACK;
	}

	/*
	 *	Now real TIME-WAIT state.
	 *
	 *	RFC 1122:
	 *	"When a connection is [...] on TIME-WAIT state [...]
	 *	[a TCP] MAY accept a new SYN from the remote TCP to
	 *	reopen the connection directly, if it:
	 *
	 *	(1)  assigns its initial sequence number for the new
	 *	connection to be larger than the largest sequence
	 *	number it used on the previous connection incarnation,
	 *	and
	 *
	 *	(2)  returns to TIME-WAIT state if the SYN turns out
	 *	to be an old duplicate".
	 */

	if (!paws_reject &&
	    (TCP_SKB_CB(skb)->seq == tcptw->tw_rcv_nxt &&
	     (TCP_SKB_CB(skb)->seq == TCP_SKB_CB(skb)->end_seq || th->rst))) {
		/* In window segment, it may be only reset or bare ack. */

		if (th->rst) {
			/* This is TIME_WAIT assassination, in two flavors.
			 * Oh well... nobody has a sufficient solution to this
			 * protocol bug yet.
			 */
			if (sysctl_tcp_rfc1337 == 0) {
kill:
				inet_twsk_deschedule_put(tw);
				return TCP_TW_SUCCESS;
			}
		}
		inet_twsk_reschedule(tw, TCP_TIMEWAIT_LEN);

		if (tmp_opt.saw_tstamp) {
			tcptw->tw_ts_recent	  = tmp_opt.rcv_tsval;
			tcptw->tw_ts_recent_stamp = get_seconds();
		}

		inet_twsk_put(tw);
		return TCP_TW_SUCCESS;
	}

	/* Out of window segment.

	   All the segments are ACKed immediately.

	   The only exception is new SYN. We accept it, if it is
	   not old duplicate and we are not in danger to be killed
	   by delayed old duplicates. RFC check is that it has
	   newer sequence number works at rates <40Mbit/sec.
	   However, if paws works, it is reliable AND even more,
	   we even may relax silly seq space cutoff.

	   RED-PEN: we violate main RFC requirement, if this SYN will appear
	   old duplicate (i.e. we receive RST in reply to SYN-ACK),
	   we must return socket to time-wait state. It is not good,
	   but not fatal yet.
	 */

	if (th->syn && !th->rst && !th->ack && !paws_reject &&
	    (after(TCP_SKB_CB(skb)->seq, tcptw->tw_rcv_nxt) ||
	     (tmp_opt.saw_tstamp &&
	      (s32)(tcptw->tw_ts_recent - tmp_opt.rcv_tsval) < 0))) {
		u32 isn = tcptw->tw_snd_nxt + 65535 + 2;
		if (isn == 0)
			isn++;
		TCP_SKB_CB(skb)->tcp_tw_isn = isn;
		return TCP_TW_SYN;
	}

	if (paws_reject)
		__NET_INC_STATS(twsk_net(tw), LINUX_MIB_PAWSESTABREJECTED);

	if (!th->rst) {
		/* In this case we must reset the TIMEWAIT timer.
		 *
		 * If it is ACKless SYN it may be both old duplicate
		 * and new good SYN with random sequence number <rcv_nxt.
		 * Do not reschedule in the last case.
		 */
		if (paws_reject || th->ack)
			inet_twsk_reschedule(tw, TCP_TIMEWAIT_LEN);

		return tcp_timewait_check_oow_rate_limit(
			tw, skb, LINUX_MIB_TCPACKSKIPPEDTIMEWAIT);
	}
	inet_twsk_put(tw);
	return TCP_TW_SUCCESS;
}