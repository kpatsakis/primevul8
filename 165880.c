static void ping_set_saddr(struct sock *sk, struct sockaddr *saddr)
{
	if (saddr->sa_family == AF_INET) {
		struct inet_sock *isk = inet_sk(sk);
		struct sockaddr_in *addr = (struct sockaddr_in *) saddr;
		isk->inet_rcv_saddr = isk->inet_saddr = addr->sin_addr.s_addr;
#if IS_ENABLED(CONFIG_IPV6)
	} else if (saddr->sa_family == AF_INET6) {
		struct sockaddr_in6 *addr = (struct sockaddr_in6 *) saddr;
		struct ipv6_pinfo *np = inet6_sk(sk);
		sk->sk_v6_rcv_saddr = np->saddr = addr->sin6_addr;
#endif
	}
}