int nfc_llcp_send_rr(struct nfc_llcp_sock *sock)
{
	struct sk_buff *skb;
	struct nfc_llcp_local *local;

	pr_debug("Send rr nr %d\n", sock->recv_n);

	local = sock->local;
	if (local == NULL)
		return -ENODEV;

	skb = llcp_allocate_pdu(sock, LLCP_PDU_RR, LLCP_SEQUENCE_SIZE);
	if (skb == NULL)
		return -ENOMEM;

	skb_put(skb, LLCP_SEQUENCE_SIZE);

	skb->data[2] = sock->recv_n;

	skb_queue_head(&local->tx_queue, skb);

	return 0;
}