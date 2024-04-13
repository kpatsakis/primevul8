static inline bool tcp_packet_delayed(const struct tcp_sock *tp)
{
	return !tp->retrans_stamp ||
	       tcp_tsopt_ecr_before(tp, tp->retrans_stamp);
}