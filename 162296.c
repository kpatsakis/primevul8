int tcp_send_rcvq(struct sock *sk, struct msghdr *msg, size_t size)
{
	struct sk_buff *skb;
	int err = -ENOMEM;
	int data_len = 0;
	bool fragstolen;

	if (size == 0)
		return 0;

	if (size > PAGE_SIZE) {
		int npages = min_t(size_t, size >> PAGE_SHIFT, MAX_SKB_FRAGS);

		data_len = npages << PAGE_SHIFT;
		size = data_len + (size & ~PAGE_MASK);
	}
	skb = alloc_skb_with_frags(size - data_len, data_len,
				   PAGE_ALLOC_COSTLY_ORDER,
				   &err, sk->sk_allocation);
	if (!skb)
		goto err;

	skb_put(skb, size - data_len);
	skb->data_len = data_len;
	skb->len = size;

	if (tcp_try_rmem_schedule(sk, skb, skb->truesize))
		goto err_free;

	err = skb_copy_datagram_from_iter(skb, 0, &msg->msg_iter, size);
	if (err)
		goto err_free;

	TCP_SKB_CB(skb)->seq = tcp_sk(sk)->rcv_nxt;
	TCP_SKB_CB(skb)->end_seq = TCP_SKB_CB(skb)->seq + size;
	TCP_SKB_CB(skb)->ack_seq = tcp_sk(sk)->snd_una - 1;

	if (tcp_queue_rcv(sk, skb, 0, &fragstolen)) {
		WARN_ON_ONCE(fragstolen); /* should not happen */
		__kfree_skb(skb);
	}
	return size;

err_free:
	kfree_skb(skb);
err:
	return err;

}