static struct sk_buff *llcp_allocate_pdu(struct nfc_llcp_sock *sock,
					 u8 cmd, u16 size)
{
	struct sk_buff *skb;
	int err;

	if (sock->ssap == 0)
		return NULL;

	skb = nfc_alloc_send_skb(sock->dev, &sock->sk, MSG_DONTWAIT,
				 size + LLCP_HEADER_SIZE, &err);
	if (skb == NULL) {
		pr_err("Could not allocate PDU\n");
		return NULL;
	}

	skb = llcp_add_header(skb, sock->dsap, sock->ssap, cmd);

	return skb;
}