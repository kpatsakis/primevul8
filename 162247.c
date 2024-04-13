static bool tcp_in_quickack_mode(struct sock *sk)
{
	const struct inet_connection_sock *icsk = inet_csk(sk);
	const struct dst_entry *dst = __sk_dst_get(sk);

	return (dst && dst_metric(dst, RTAX_QUICKACK)) ||
		(icsk->icsk_ack.quick && !icsk->icsk_ack.pingpong);
}