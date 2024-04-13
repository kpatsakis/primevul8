static bool tcp_any_retrans_done(const struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	struct sk_buff *skb;

	if (tp->retrans_out)
		return true;

	skb = tcp_write_queue_head(sk);
	if (unlikely(skb && TCP_SKB_CB(skb)->sacked & TCPCB_EVER_RETRANS))
		return true;

	return false;
}