void tcp_synack_rtt_meas(struct sock *sk, struct request_sock *req)
{
	long rtt_us = -1L;

	if (req && !req->num_retrans && tcp_rsk(req)->snt_synack.v64) {
		struct skb_mstamp now;

		skb_mstamp_get(&now);
		rtt_us = skb_mstamp_us_delta(&now, &tcp_rsk(req)->snt_synack);
	}

	tcp_ack_update_rtt(sk, FLAG_SYN_ACKED, rtt_us, -1L, rtt_us);
}