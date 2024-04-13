static int ppp_mp_explode(struct ppp *ppp, struct sk_buff *skb)
{
	int len, fragsize;
	int i, bits, hdrlen, mtu;
	int flen;
	int navail, nfree;
	int nbigger;
	unsigned char *p, *q;
	struct list_head *list;
	struct channel *pch;
	struct sk_buff *frag;
	struct ppp_channel *chan;

	nfree = 0;	/* # channels which have no packet already queued */
	navail = 0;	/* total # of usable channels (not deregistered) */
	hdrlen = (ppp->flags & SC_MP_XSHORTSEQ)? MPHDRLEN_SSN: MPHDRLEN;
	i = 0;
	list_for_each_entry(pch, &ppp->channels, clist) {
		navail += pch->avail = (pch->chan != NULL);
		if (pch->avail) {
			if (skb_queue_empty(&pch->file.xq) ||
			    !pch->had_frag) {
				pch->avail = 2;
				++nfree;
			}
			if (!pch->had_frag && i < ppp->nxchan)
				ppp->nxchan = i;
		}
		++i;
	}

	/*
	 * Don't start sending this packet unless at least half of
	 * the channels are free.  This gives much better TCP
	 * performance if we have a lot of channels.
	 */
	if (nfree == 0 || nfree < navail / 2)
		return 0;	/* can't take now, leave it in xmit_pending */

	/* Do protocol field compression (XXX this should be optional) */
	p = skb->data;
	len = skb->len;
	if (*p == 0) {
		++p;
		--len;
	}

	/*
	 * Decide on fragment size.
	 * We create a fragment for each free channel regardless of
	 * how small they are (i.e. even 0 length) in order to minimize
	 * the time that it will take to detect when a channel drops
	 * a fragment.
	 */
	fragsize = len;
	if (nfree > 1)
		fragsize = DIV_ROUND_UP(fragsize, nfree);
	/* nbigger channels get fragsize bytes, the rest get fragsize-1,
	   except if nbigger==0, then they all get fragsize. */
	nbigger = len % nfree;

	/* skip to the channel after the one we last used
	   and start at that one */
	list = &ppp->channels;
	for (i = 0; i < ppp->nxchan; ++i) {
		list = list->next;
		if (list == &ppp->channels) {
			i = 0;
			break;
		}
	}

	/* create a fragment for each channel */
	bits = B;
	while (nfree > 0 || len > 0) {
		list = list->next;
		if (list == &ppp->channels) {
			i = 0;
			continue;
		}
		pch = list_entry(list, struct channel, clist);
		++i;
		if (!pch->avail)
			continue;

		/*
		 * Skip this channel if it has a fragment pending already and
		 * we haven't given a fragment to all of the free channels.
		 */
		if (pch->avail == 1) {
			if (nfree > 0)
				continue;
		} else {
			--nfree;
			pch->avail = 1;
		}

		/* check the channel's mtu and whether it is still attached. */
		spin_lock_bh(&pch->downl);
		if (pch->chan == NULL) {
			/* can't use this channel, it's being deregistered */
			spin_unlock_bh(&pch->downl);
			pch->avail = 0;
			if (--navail == 0)
				break;
			continue;
		}

		/*
		 * Create a fragment for this channel of
		 * min(max(mtu+2-hdrlen, 4), fragsize, len) bytes.
		 * If mtu+2-hdrlen < 4, that is a ridiculously small
		 * MTU, so we use mtu = 2 + hdrlen.
		 */
		if (fragsize > len)
			fragsize = len;
		flen = fragsize;
		mtu = pch->chan->mtu + 2 - hdrlen;
		if (mtu < 4)
			mtu = 4;
		if (flen > mtu)
			flen = mtu;
		if (flen == len && nfree == 0)
			bits |= E;
		frag = alloc_skb(flen + hdrlen + (flen == 0), GFP_ATOMIC);
		if (!frag)
			goto noskb;
		q = skb_put(frag, flen + hdrlen);

		/* make the MP header */
		q[0] = PPP_MP >> 8;
		q[1] = PPP_MP;
		if (ppp->flags & SC_MP_XSHORTSEQ) {
			q[2] = bits + ((ppp->nxseq >> 8) & 0xf);
			q[3] = ppp->nxseq;
		} else {
			q[2] = bits;
			q[3] = ppp->nxseq >> 16;
			q[4] = ppp->nxseq >> 8;
			q[5] = ppp->nxseq;
		}

		/*
		 * Copy the data in.
		 * Unfortunately there is a bug in older versions of
		 * the Linux PPP multilink reconstruction code where it
		 * drops 0-length fragments.  Therefore we make sure the
		 * fragment has at least one byte of data.  Any bytes
		 * we add in this situation will end up as padding on the
		 * end of the reconstructed packet.
		 */
		if (flen == 0)
			*skb_put(frag, 1) = 0;
		else
			memcpy(q + hdrlen, p, flen);

		/* try to send it down the channel */
		chan = pch->chan;
		if (!skb_queue_empty(&pch->file.xq) ||
		    !chan->ops->start_xmit(chan, frag))
			skb_queue_tail(&pch->file.xq, frag);
		pch->had_frag = 1;
		p += flen;
		len -= flen;
		++ppp->nxseq;
		bits = 0;
		spin_unlock_bh(&pch->downl);

		if (--nbigger == 0 && fragsize > 0)
			--fragsize;
	}
	ppp->nxchan = i;

	return 1;

 noskb:
	spin_unlock_bh(&pch->downl);
	if (ppp->debug & 1)
		printk(KERN_ERR "PPP: no memory (fragment)\n");
	++ppp->dev->stats.tx_errors;
	++ppp->nxseq;
	return 1;	/* abandon the frame */
}