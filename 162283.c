static void tcp_sndbuf_expand(struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	int sndmem, per_mss;
	u32 nr_segs;

	/* Worst case is non GSO/TSO : each frame consumes one skb
	 * and skb->head is kmalloced using power of two area of memory
	 */
	per_mss = max_t(u32, tp->rx_opt.mss_clamp, tp->mss_cache) +
		  MAX_TCP_HEADER +
		  SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

	per_mss = roundup_pow_of_two(per_mss) +
		  SKB_DATA_ALIGN(sizeof(struct sk_buff));

	nr_segs = max_t(u32, TCP_INIT_CWND, tp->snd_cwnd);
	nr_segs = max_t(u32, nr_segs, tp->reordering + 1);

	/* Fast Recovery (RFC 5681 3.2) :
	 * Cubic needs 1.7 factor, rounded to 2 to include
	 * extra cushion (application might react slowly to POLLOUT)
	 */
	sndmem = 2 * nr_segs * per_mss;

	if (sk->sk_sndbuf < sndmem)
		sk->sk_sndbuf = min(sndmem, sysctl_tcp_wmem[2]);
}