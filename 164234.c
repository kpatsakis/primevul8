static int ltalk_rcv(struct sk_buff *skb, struct net_device *dev,
		     struct packet_type *pt, struct net_device *orig_dev)
{
	if (!net_eq(dev_net(dev), &init_net))
		goto freeit;

	/* Expand any short form frames */
	if (skb_mac_header(skb)[2] == 1) {
		struct ddpehdr *ddp;
		/* Find our address */
		struct atalk_addr *ap = atalk_find_dev_addr(dev);

		if (!ap || skb->len < sizeof(__be16) || skb->len > 1023)
			goto freeit;

		/* Don't mangle buffer if shared */
		if (!(skb = skb_share_check(skb, GFP_ATOMIC)))
			return 0;

		/*
		 * The push leaves us with a ddephdr not an shdr, and
		 * handily the port bytes in the right place preset.
		 */
		ddp = skb_push(skb, sizeof(*ddp) - 4);

		/* Now fill in the long header */

		/*
		 * These two first. The mac overlays the new source/dest
		 * network information so we MUST copy these before
		 * we write the network numbers !
		 */

		ddp->deh_dnode = skb_mac_header(skb)[0];     /* From physical header */
		ddp->deh_snode = skb_mac_header(skb)[1];     /* From physical header */

		ddp->deh_dnet  = ap->s_net;	/* Network number */
		ddp->deh_snet  = ap->s_net;
		ddp->deh_sum   = 0;		/* No checksum */
		/*
		 * Not sure about this bit...
		 */
		/* Non routable, so force a drop if we slip up later */
		ddp->deh_len_hops = htons(skb->len + (DDP_MAXHOPS << 10));
	}
	skb_reset_transport_header(skb);

	return atalk_rcv(skb, dev, pt, orig_dev);
freeit:
	kfree_skb(skb);
	return 0;
}