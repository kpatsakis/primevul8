ppp_mp_reconstruct(struct ppp *ppp)
{
	u32 seq = ppp->nextseq;
	u32 minseq = ppp->minseq;
	struct sk_buff_head *list = &ppp->mrq;
	struct sk_buff *p, *next;
	struct sk_buff *head, *tail;
	struct sk_buff *skb = NULL;
	int lost = 0, len = 0;

	if (ppp->mrru == 0)	/* do nothing until mrru is set */
		return NULL;
	head = list->next;
	tail = NULL;
	for (p = head; p != (struct sk_buff *) list; p = next) {
		next = p->next;
		if (seq_before(p->sequence, seq)) {
			/* this can't happen, anyway ignore the skb */
			printk(KERN_ERR "ppp_mp_reconstruct bad seq %u < %u\n",
			       p->sequence, seq);
			head = next;
			continue;
		}
		if (p->sequence != seq) {
			/* Fragment `seq' is missing.  If it is after
			   minseq, it might arrive later, so stop here. */
			if (seq_after(seq, minseq))
				break;
			/* Fragment `seq' is lost, keep going. */
			lost = 1;
			seq = seq_before(minseq, p->sequence)?
				minseq + 1: p->sequence;
			next = p;
			continue;
		}

		/*
		 * At this point we know that all the fragments from
		 * ppp->nextseq to seq are either present or lost.
		 * Also, there are no complete packets in the queue
		 * that have no missing fragments and end before this
		 * fragment.
		 */

		/* B bit set indicates this fragment starts a packet */
		if (p->BEbits & B) {
			head = p;
			lost = 0;
			len = 0;
		}

		len += p->len;

		/* Got a complete packet yet? */
		if (lost == 0 && (p->BEbits & E) && (head->BEbits & B)) {
			if (len > ppp->mrru + 2) {
				++ppp->dev->stats.rx_length_errors;
				printk(KERN_DEBUG "PPP: reconstructed packet"
				       " is too long (%d)\n", len);
			} else if (p == head) {
				/* fragment is complete packet - reuse skb */
				tail = p;
				skb = skb_get(p);
				break;
			} else if ((skb = dev_alloc_skb(len)) == NULL) {
				++ppp->dev->stats.rx_missed_errors;
				printk(KERN_DEBUG "PPP: no memory for "
				       "reconstructed packet");
			} else {
				tail = p;
				break;
			}
			ppp->nextseq = seq + 1;
		}

		/*
		 * If this is the ending fragment of a packet,
		 * and we haven't found a complete valid packet yet,
		 * we can discard up to and including this fragment.
		 */
		if (p->BEbits & E)
			head = next;

		++seq;
	}

	/* If we have a complete packet, copy it all into one skb. */
	if (tail != NULL) {
		/* If we have discarded any fragments,
		   signal a receive error. */
		if (head->sequence != ppp->nextseq) {
			if (ppp->debug & 1)
				printk(KERN_DEBUG "  missed pkts %u..%u\n",
				       ppp->nextseq, head->sequence-1);
			++ppp->dev->stats.rx_dropped;
			ppp_receive_error(ppp);
		}

		if (head != tail)
			/* copy to a single skb */
			for (p = head; p != tail->next; p = p->next)
				skb_copy_bits(p, 0, skb_put(skb, p->len), p->len);
		ppp->nextseq = tail->sequence + 1;
		head = tail->next;
	}

	/* Discard all the skbuffs that we have copied the data out of
	   or that we can't use. */
	while ((p = list->next) != head) {
		__skb_unlink(p, list);
		kfree_skb(p);
	}

	return skb;
}