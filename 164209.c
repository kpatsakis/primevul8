static int atalk_route_packet(struct sk_buff *skb, struct net_device *dev,
			      struct ddpehdr *ddp, __u16 len_hops, int origlen)
{
	struct atalk_route *rt;
	struct atalk_addr ta;

	/*
	 * Don't route multicast, etc., packets, or packets sent to "this
	 * network"
	 */
	if (skb->pkt_type != PACKET_HOST || !ddp->deh_dnet) {
		/*
		 * FIXME:
		 *
		 * Can it ever happen that a packet is from a PPP iface and
		 * needs to be broadcast onto the default network?
		 */
		if (dev->type == ARPHRD_PPP)
			printk(KERN_DEBUG "AppleTalk: didn't forward broadcast "
					  "packet received from PPP iface\n");
		goto free_it;
	}

	ta.s_net  = ddp->deh_dnet;
	ta.s_node = ddp->deh_dnode;

	/* Route the packet */
	rt = atrtr_find(&ta);
	/* increment hops count */
	len_hops += 1 << 10;
	if (!rt || !(len_hops & (15 << 10)))
		goto free_it;

	/* FIXME: use skb->cb to be able to use shared skbs */

	/*
	 * Route goes through another gateway, so set the target to the
	 * gateway instead.
	 */

	if (rt->flags & RTF_GATEWAY) {
		ta.s_net  = rt->gateway.s_net;
		ta.s_node = rt->gateway.s_node;
	}

	/* Fix up skb->len field */
	skb_trim(skb, min_t(unsigned int, origlen,
			    (rt->dev->hard_header_len +
			     ddp_dl->header_length + (len_hops & 1023))));

	/* FIXME: use skb->cb to be able to use shared skbs */
	ddp->deh_len_hops = htons(len_hops);

	/*
	 * Send the buffer onwards
	 *
	 * Now we must always be careful. If it's come from LocalTalk to
	 * EtherTalk it might not fit
	 *
	 * Order matters here: If a packet has to be copied to make a new
	 * headroom (rare hopefully) then it won't need unsharing.
	 *
	 * Note. ddp-> becomes invalid at the realloc.
	 */
	if (skb_headroom(skb) < 22) {
		/* 22 bytes - 12 ether, 2 len, 3 802.2 5 snap */
		struct sk_buff *nskb = skb_realloc_headroom(skb, 32);
		kfree_skb(skb);
		skb = nskb;
	} else
		skb = skb_unshare(skb, GFP_ATOMIC);

	/*
	 * If the buffer didn't vanish into the lack of space bitbucket we can
	 * send it.
	 */
	if (skb == NULL)
		goto drop;

	if (aarp_send_ddp(rt->dev, skb, &ta, NULL) == NET_XMIT_DROP)
		return NET_RX_DROP;
	return NET_RX_SUCCESS;
free_it:
	kfree_skb(skb);
drop:
	return NET_RX_DROP;
}