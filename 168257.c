static struct sk_buff *nfc_llcp_allocate_snl(struct nfc_llcp_local *local,
					     size_t tlv_length)
{
	struct sk_buff *skb;
	struct nfc_dev *dev;
	u16 size = 0;

	if (local == NULL)
		return ERR_PTR(-ENODEV);

	dev = local->dev;
	if (dev == NULL)
		return ERR_PTR(-ENODEV);

	size += LLCP_HEADER_SIZE;
	size += dev->tx_headroom + dev->tx_tailroom + NFC_HEADER_SIZE;
	size += tlv_length;

	skb = alloc_skb(size, GFP_KERNEL);
	if (skb == NULL)
		return ERR_PTR(-ENOMEM);

	skb_reserve(skb, dev->tx_headroom + NFC_HEADER_SIZE);

	skb = llcp_add_header(skb, LLCP_SAP_SDP, LLCP_SAP_SDP, LLCP_PDU_SNL);

	return skb;
}