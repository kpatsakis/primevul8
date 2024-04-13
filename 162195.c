void tcp_rcv_space_adjust(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	int time;
	int copied;

	time = tcp_time_stamp - tp->rcvq_space.time;
	if (time < (tp->rcv_rtt_est.rtt >> 3) || tp->rcv_rtt_est.rtt == 0)
		return;

	/* Number of bytes copied to user in last RTT */
	copied = tp->copied_seq - tp->rcvq_space.seq;
	if (copied <= tp->rcvq_space.space)
		goto new_measure;

	/* A bit of theory :
	 * copied = bytes received in previous RTT, our base window
	 * To cope with packet losses, we need a 2x factor
	 * To cope with slow start, and sender growing its cwin by 100 %
	 * every RTT, we need a 4x factor, because the ACK we are sending
	 * now is for the next RTT, not the current one :
	 * <prev RTT . ><current RTT .. ><next RTT .... >
	 */

	if (sysctl_tcp_moderate_rcvbuf &&
	    !(sk->sk_userlocks & SOCK_RCVBUF_LOCK)) {
		int rcvwin, rcvmem, rcvbuf;

		/* minimal window to cope with packet losses, assuming
		 * steady state. Add some cushion because of small variations.
		 */
		rcvwin = (copied << 1) + 16 * tp->advmss;

		/* If rate increased by 25%,
		 *	assume slow start, rcvwin = 3 * copied
		 * If rate increased by 50%,
		 *	assume sender can use 2x growth, rcvwin = 4 * copied
		 */
		if (copied >=
		    tp->rcvq_space.space + (tp->rcvq_space.space >> 2)) {
			if (copied >=
			    tp->rcvq_space.space + (tp->rcvq_space.space >> 1))
				rcvwin <<= 1;
			else
				rcvwin += (rcvwin >> 1);
		}

		rcvmem = SKB_TRUESIZE(tp->advmss + MAX_TCP_HEADER);
		while (tcp_win_from_space(rcvmem) < tp->advmss)
			rcvmem += 128;

		rcvbuf = min(rcvwin / tp->advmss * rcvmem, sysctl_tcp_rmem[2]);
		if (rcvbuf > sk->sk_rcvbuf) {
			sk->sk_rcvbuf = rcvbuf;

			/* Make the window clamp follow along.  */
			tp->window_clamp = rcvwin;
		}
	}
	tp->rcvq_space.space = copied;

new_measure:
	tp->rcvq_space.seq = tp->copied_seq;
	tp->rcvq_space.time = tcp_time_stamp;
}