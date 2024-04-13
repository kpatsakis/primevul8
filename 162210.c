static void tcp_new_space(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);

	if (tcp_should_expand_sndbuf(sk)) {
		tcp_sndbuf_expand(sk);
		tp->snd_cwnd_stamp = tcp_time_stamp;
	}

	sk->sk_write_space(sk);
}