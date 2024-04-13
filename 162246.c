static inline int tcp_dupack_heuristics(const struct tcp_sock *tp)
{
	return tcp_is_fack(tp) ? tp->fackets_out : tp->sacked_out + 1;
}