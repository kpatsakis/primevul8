struct nfc_llcp_sdp_tlv *nfc_llcp_build_sdres_tlv(u8 tid, u8 sap)
{
	struct nfc_llcp_sdp_tlv *sdres;
	u8 value[2];

	sdres = kzalloc(sizeof(struct nfc_llcp_sdp_tlv), GFP_KERNEL);
	if (sdres == NULL)
		return NULL;

	value[0] = tid;
	value[1] = sap;

	sdres->tlv = nfc_llcp_build_tlv(LLCP_TLV_SDRES, value, 2,
					&sdres->tlv_len);
	if (sdres->tlv == NULL) {
		kfree(sdres);
		return NULL;
	}

	sdres->tid = tid;
	sdres->sap = sap;

	INIT_HLIST_NODE(&sdres->node);

	return sdres;
}