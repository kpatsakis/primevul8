static void tcp_ecn_check_ce(struct tcp_sock *tp, const struct sk_buff *skb)
{
	if (tp->ecn_flags & TCP_ECN_OK)
		__tcp_ecn_check_ce(tp, skb);
}