static void tcp_fixup_rcvbuf(struct sock *sk)
{
	u32 mss = tcp_sk(sk)->advmss;
	int rcvmem;

	rcvmem = 2 * SKB_TRUESIZE(mss + MAX_TCP_HEADER) *
		 tcp_default_init_rwnd(mss);

	/* Dynamic Right Sizing (DRS) has 2 to 3 RTT latency
	 * Allow enough cushion so that sender is not limited by our window
	 */
	if (sysctl_tcp_moderate_rcvbuf)
		rcvmem <<= 2;

	if (sk->sk_rcvbuf < rcvmem)
		sk->sk_rcvbuf = min(rcvmem, sysctl_tcp_rmem[2]);
}