static void ping_clear_saddr(struct sock *sk, int dif)
{
	sk->sk_bound_dev_if = dif;
	if (sk->sk_family == AF_INET) {
		struct inet_sock *isk = inet_sk(sk);
		isk->inet_rcv_saddr = isk->inet_saddr = 0;
#if IS_ENABLED(CONFIG_IPV6)
	} else if (sk->sk_family == AF_INET6) {
		struct ipv6_pinfo *np = inet6_sk(sk);
		memset(&sk->sk_v6_rcv_saddr, 0, sizeof(sk->sk_v6_rcv_saddr));
		memset(&np->saddr, 0, sizeof(np->saddr));
#endif
	}
}