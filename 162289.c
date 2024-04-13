static int tcp_try_rmem_schedule(struct sock *sk, struct sk_buff *skb,
				 unsigned int size)
{
	if (atomic_read(&sk->sk_rmem_alloc) > sk->sk_rcvbuf ||
	    !sk_rmem_schedule(sk, skb, size)) {

		if (tcp_prune_queue(sk) < 0)
			return -1;

		if (!sk_rmem_schedule(sk, skb, size)) {
			if (!tcp_prune_ofo_queue(sk))
				return -1;

			if (!sk_rmem_schedule(sk, skb, size))
				return -1;
		}
	}
	return 0;
}