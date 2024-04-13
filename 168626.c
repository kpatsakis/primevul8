tcp_timewait_check_oow_rate_limit(struct inet_timewait_sock *tw,
				  const struct sk_buff *skb, int mib_idx)
{
	struct tcp_timewait_sock *tcptw = tcp_twsk((struct sock *)tw);

	if (!tcp_oow_rate_limited(twsk_net(tw), skb, mib_idx,
				  &tcptw->tw_last_oow_ack_time)) {
		/* Send ACK. Note, we do not put the bucket,
		 * it will be released by caller.
		 */
		return TCP_TW_ACK;
	}

	/* We are rate-limiting, so just release the tw sock and drop skb. */
	inet_twsk_put(tw);
	return TCP_TW_SUCCESS;
}