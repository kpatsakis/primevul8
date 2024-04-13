ppp_send_frame(struct ppp *ppp, struct sk_buff *skb)
{
	int proto = PPP_PROTO(skb);
	struct sk_buff *new_skb;
	int len;
	unsigned char *cp;

	if (proto < 0x8000) {
#ifdef CONFIG_PPP_FILTER
		/* check if we should pass this packet */
		/* the filter instructions are constructed assuming
		   a four-byte PPP header on each packet */
		*skb_push(skb, 2) = 1;
		if (ppp->pass_filter
		    && sk_run_filter(skb, ppp->pass_filter,
				     ppp->pass_len) == 0) {
			if (ppp->debug & 1)
				printk(KERN_DEBUG "PPP: outbound frame not passed\n");
			kfree_skb(skb);
			return;
		}
		/* if this packet passes the active filter, record the time */
		if (!(ppp->active_filter
		      && sk_run_filter(skb, ppp->active_filter,
				       ppp->active_len) == 0))
			ppp->last_xmit = jiffies;
		skb_pull(skb, 2);
#else
		/* for data packets, record the time */
		ppp->last_xmit = jiffies;
#endif /* CONFIG_PPP_FILTER */
	}

	++ppp->dev->stats.tx_packets;
	ppp->dev->stats.tx_bytes += skb->len - 2;

	switch (proto) {
	case PPP_IP:
		if (!ppp->vj || (ppp->flags & SC_COMP_TCP) == 0)
			break;
		/* try to do VJ TCP header compression */
		new_skb = alloc_skb(skb->len + ppp->dev->hard_header_len - 2,
				    GFP_ATOMIC);
		if (!new_skb) {
			printk(KERN_ERR "PPP: no memory (VJ comp pkt)\n");
			goto drop;
		}
		skb_reserve(new_skb, ppp->dev->hard_header_len - 2);
		cp = skb->data + 2;
		len = slhc_compress(ppp->vj, cp, skb->len - 2,
				    new_skb->data + 2, &cp,
				    !(ppp->flags & SC_NO_TCP_CCID));
		if (cp == skb->data + 2) {
			/* didn't compress */
			kfree_skb(new_skb);
		} else {
			if (cp[0] & SL_TYPE_COMPRESSED_TCP) {
				proto = PPP_VJC_COMP;
				cp[0] &= ~SL_TYPE_COMPRESSED_TCP;
			} else {
				proto = PPP_VJC_UNCOMP;
				cp[0] = skb->data[2];
			}
			kfree_skb(skb);
			skb = new_skb;
			cp = skb_put(skb, len + 2);
			cp[0] = 0;
			cp[1] = proto;
		}
		break;

	case PPP_CCP:
		/* peek at outbound CCP frames */
		ppp_ccp_peek(ppp, skb, 0);
		break;
	}

	/* try to do packet compression */
	if ((ppp->xstate & SC_COMP_RUN) && ppp->xc_state
	    && proto != PPP_LCP && proto != PPP_CCP) {
		if (!(ppp->flags & SC_CCP_UP) && (ppp->flags & SC_MUST_COMP)) {
			if (net_ratelimit())
				printk(KERN_ERR "ppp: compression required but down - pkt dropped.\n");
			goto drop;
		}
		skb = pad_compress_skb(ppp, skb);
		if (!skb)
			goto drop;
	}

	/*
	 * If we are waiting for traffic (demand dialling),
	 * queue it up for pppd to receive.
	 */
	if (ppp->flags & SC_LOOP_TRAFFIC) {
		if (ppp->file.rq.qlen > PPP_MAX_RQLEN)
			goto drop;
		skb_queue_tail(&ppp->file.rq, skb);
		wake_up_interruptible(&ppp->file.rwait);
		return;
	}

	ppp->xmit_pending = skb;
	ppp_push(ppp);
	return;

 drop:
	if (skb)
		kfree_skb(skb);
	++ppp->dev->stats.tx_errors;
}