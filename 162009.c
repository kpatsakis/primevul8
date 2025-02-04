static struct sk_buff *udp_gro_receive_segment(struct list_head *head,
					       struct sk_buff *skb)
{
	struct udphdr *uh = udp_hdr(skb);
	struct sk_buff *pp = NULL;
	struct udphdr *uh2;
	struct sk_buff *p;
	unsigned int ulen;

	/* requires non zero csum, for symmetry with GSO */
	if (!uh->check) {
		NAPI_GRO_CB(skb)->flush = 1;
		return NULL;
	}

	/* Do not deal with padded or malicious packets, sorry ! */
	ulen = ntohs(uh->len);
	if (ulen <= sizeof(*uh) || ulen != skb_gro_len(skb)) {
		NAPI_GRO_CB(skb)->flush = 1;
		return NULL;
	}
	/* pull encapsulating udp header */
	skb_gro_pull(skb, sizeof(struct udphdr));
	skb_gro_postpull_rcsum(skb, uh, sizeof(struct udphdr));

	list_for_each_entry(p, head, list) {
		if (!NAPI_GRO_CB(p)->same_flow)
			continue;

		uh2 = udp_hdr(p);

		/* Match ports only, as csum is always non zero */
		if ((*(u32 *)&uh->source != *(u32 *)&uh2->source)) {
			NAPI_GRO_CB(p)->same_flow = 0;
			continue;
		}

		/* Terminate the flow on len mismatch or if it grow "too much".
		 * Under small packet flood GRO count could elsewhere grow a lot
		 * leading to excessive truesize values.
		 * On len mismatch merge the first packet shorter than gso_size,
		 * otherwise complete the GRO packet.
		 */
		if (ulen > ntohs(uh2->len) || skb_gro_receive(p, skb) ||
		    ulen != ntohs(uh2->len) ||
		    NAPI_GRO_CB(p)->count >= UDP_GRO_CNT_MAX)
			pp = p;

		return pp;
	}

	/* mismatch, but we never need to flush */
	return NULL;
}