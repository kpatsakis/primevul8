void nfc_llcp_free_sdp_tlv_list(struct hlist_head *head)
{
	struct nfc_llcp_sdp_tlv *sdp;
	struct hlist_node *n;

	hlist_for_each_entry_safe(sdp, n, head, node) {
		hlist_del(&sdp->node);

		nfc_llcp_free_sdp_tlv(sdp);
	}
}