static bool tcp_pause_early_retransmit(struct sock *sk, int flag)
{
	struct tcp_sock *tp = tcp_sk(sk);
	unsigned long delay;

	/* Delay early retransmit and entering fast recovery for
	 * max(RTT/4, 2msec) unless ack has ECE mark, no RTT samples
	 * available, or RTO is scheduled to fire first.
	 */
	if (sysctl_tcp_early_retrans < 2 || sysctl_tcp_early_retrans > 3 ||
	    (flag & FLAG_ECE) || !tp->srtt_us)
		return false;

	delay = max(usecs_to_jiffies(tp->srtt_us >> 5),
		    msecs_to_jiffies(2));

	if (!time_after(inet_csk(sk)->icsk_timeout, (jiffies + delay)))
		return false;

	inet_csk_reset_xmit_timer(sk, ICSK_TIME_EARLY_RETRANS, delay,
				  TCP_RTO_MAX);
	return true;
}