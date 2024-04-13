tcp_collapse(struct sock *sk, struct sk_buff_head *list,
	     struct sk_buff *head, struct sk_buff *tail,
	     u32 start, u32 end)
{
	struct sk_buff *skb, *n;
	bool end_of_skbs;

	/* First, check that queue is collapsible and find
	 * the point where collapsing can be useful. */
	skb = head;
restart:
	end_of_skbs = true;
	skb_queue_walk_from_safe(list, skb, n) {
		if (skb == tail)
			break;
		/* No new bits? It is possible on ofo queue. */
		if (!before(start, TCP_SKB_CB(skb)->end_seq)) {
			skb = tcp_collapse_one(sk, skb, list);
			if (!skb)
				break;
			goto restart;
		}

		/* The first skb to collapse is:
		 * - not SYN/FIN and
		 * - bloated or contains data before "start" or
		 *   overlaps to the next one.
		 */
		if (!(TCP_SKB_CB(skb)->tcp_flags & (TCPHDR_SYN | TCPHDR_FIN)) &&
		    (tcp_win_from_space(skb->truesize) > skb->len ||
		     before(TCP_SKB_CB(skb)->seq, start))) {
			end_of_skbs = false;
			break;
		}

		if (!skb_queue_is_last(list, skb)) {
			struct sk_buff *next = skb_queue_next(list, skb);
			if (next != tail &&
			    TCP_SKB_CB(skb)->end_seq != TCP_SKB_CB(next)->seq) {
				end_of_skbs = false;
				break;
			}
		}

		/* Decided to skip this, advance start seq. */
		start = TCP_SKB_CB(skb)->end_seq;
	}
	if (end_of_skbs ||
	    (TCP_SKB_CB(skb)->tcp_flags & (TCPHDR_SYN | TCPHDR_FIN)))
		return;

	while (before(start, end)) {
		int copy = min_t(int, SKB_MAX_ORDER(0, 0), end - start);
		struct sk_buff *nskb;

		nskb = alloc_skb(copy, GFP_ATOMIC);
		if (!nskb)
			return;

		memcpy(nskb->cb, skb->cb, sizeof(skb->cb));
		TCP_SKB_CB(nskb)->seq = TCP_SKB_CB(nskb)->end_seq = start;
		__skb_queue_before(list, skb, nskb);
		skb_set_owner_r(nskb, sk);

		/* Copy data, releasing collapsed skbs. */
		while (copy > 0) {
			int offset = start - TCP_SKB_CB(skb)->seq;
			int size = TCP_SKB_CB(skb)->end_seq - start;

			BUG_ON(offset < 0);
			if (size > 0) {
				size = min(copy, size);
				if (skb_copy_bits(skb, offset, skb_put(nskb, size), size))
					BUG();
				TCP_SKB_CB(nskb)->end_seq += size;
				copy -= size;
				start += size;
			}
			if (!before(start, TCP_SKB_CB(skb)->end_seq)) {
				skb = tcp_collapse_one(sk, skb, list);
				if (!skb ||
				    skb == tail ||
				    (TCP_SKB_CB(skb)->tcp_flags & (TCPHDR_SYN | TCPHDR_FIN)))
					return;
			}
		}
	}
}