static inline void tcp_end_cwnd_reduction(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);

	/* Reset cwnd to ssthresh in CWR or Recovery (unless it's undone) */
	if (inet_csk(sk)->icsk_ca_state == TCP_CA_CWR ||
	    (tp->undo_marker && tp->snd_ssthresh < TCP_INFINITE_SSTHRESH)) {
		tp->snd_cwnd = tp->snd_ssthresh;
		tp->snd_cwnd_stamp = tcp_time_stamp;
	}
	tcp_ca_event(sk, CA_EVENT_COMPLETE_CWR);
}