void tcp_rearm_rto(struct sock *sk)
{
	const struct inet_connection_sock *icsk = inet_csk(sk);
	struct tcp_sock *tp = tcp_sk(sk);

	/* If the retrans timer is currently being used by Fast Open
	 * for SYN-ACK retrans purpose, stay put.
	 */
	if (tp->fastopen_rsk)
		return;

	if (!tp->packets_out) {
		inet_csk_clear_xmit_timer(sk, ICSK_TIME_RETRANS);
	} else {
		u32 rto = inet_csk(sk)->icsk_rto;
		/* Offset the time elapsed after installing regular RTO */
		if (icsk->icsk_pending == ICSK_TIME_EARLY_RETRANS ||
		    icsk->icsk_pending == ICSK_TIME_LOSS_PROBE) {
			struct sk_buff *skb = tcp_write_queue_head(sk);
			const u32 rto_time_stamp =
				tcp_skb_timestamp(skb) + rto;
			s32 delta = (s32)(rto_time_stamp - tcp_time_stamp);
			/* delta may not be positive if the socket is locked
			 * when the retrans timer fires and is rescheduled.
			 */
			if (delta > 0)
				rto = delta;
		}
		inet_csk_reset_xmit_timer(sk, ICSK_TIME_RETRANS, rto,
					  TCP_RTO_MAX);
	}
}