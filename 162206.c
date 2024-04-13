void tcp_finish_connect(struct sock *sk, struct sk_buff *skb)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct inet_connection_sock *icsk = inet_csk(sk);

	tcp_set_state(sk, TCP_ESTABLISHED);

	if (skb) {
		icsk->icsk_af_ops->sk_rx_dst_set(sk, skb);
		security_inet_conn_established(sk, skb);
	}

	/* Make sure socket is routed, for correct metrics.  */
	icsk->icsk_af_ops->rebuild_header(sk);

	tcp_init_metrics(sk);

	tcp_init_congestion_control(sk);

	/* Prevent spurious tcp_cwnd_restart() on first data
	 * packet.
	 */
	tp->lsndtime = tcp_time_stamp;

	tcp_init_buffer_space(sk);

	if (sock_flag(sk, SOCK_KEEPOPEN))
		inet_csk_reset_keepalive_timer(sk, keepalive_time_when(tp));

	if (!tp->rx_opt.snd_wscale)
		__tcp_fast_path_on(tp, tp->snd_wnd);
	else
		tp->pred_flags = 0;

	if (!sock_flag(sk, SOCK_DEAD)) {
		sk->sk_state_change(sk);
		sk_wake_async(sk, SOCK_WAKE_IO, POLL_OUT);
	}
}