mwifiex_update_uap_custom_ie(struct mwifiex_private *priv,
			     struct mwifiex_ie *beacon_ie, u16 *beacon_idx,
			     struct mwifiex_ie *pr_ie, u16 *probe_idx,
			     struct mwifiex_ie *ar_ie, u16 *assoc_idx)
{
	struct mwifiex_ie_list *ap_custom_ie;
	u8 *pos;
	u16 len;
	int ret;

	ap_custom_ie = kzalloc(sizeof(*ap_custom_ie), GFP_KERNEL);
	if (!ap_custom_ie)
		return -ENOMEM;

	ap_custom_ie->type = cpu_to_le16(TLV_TYPE_MGMT_IE);
	pos = (u8 *)ap_custom_ie->ie_list;

	if (beacon_ie) {
		len = sizeof(struct mwifiex_ie) - IEEE_MAX_IE_SIZE +
		      le16_to_cpu(beacon_ie->ie_length);
		memcpy(pos, beacon_ie, len);
		pos += len;
		le16_unaligned_add_cpu(&ap_custom_ie->len, len);
	}
	if (pr_ie) {
		len = sizeof(struct mwifiex_ie) - IEEE_MAX_IE_SIZE +
		      le16_to_cpu(pr_ie->ie_length);
		memcpy(pos, pr_ie, len);
		pos += len;
		le16_unaligned_add_cpu(&ap_custom_ie->len, len);
	}
	if (ar_ie) {
		len = sizeof(struct mwifiex_ie) - IEEE_MAX_IE_SIZE +
		      le16_to_cpu(ar_ie->ie_length);
		memcpy(pos, ar_ie, len);
		pos += len;
		le16_unaligned_add_cpu(&ap_custom_ie->len, len);
	}

	ret = mwifiex_update_autoindex_ies(priv, ap_custom_ie);

	pos = (u8 *)(&ap_custom_ie->ie_list[0].ie_index);
	if (beacon_ie && *beacon_idx == MWIFIEX_AUTO_IDX_MASK) {
		/* save beacon ie index after auto-indexing */
		*beacon_idx = le16_to_cpu(ap_custom_ie->ie_list[0].ie_index);
		len = sizeof(*beacon_ie) - IEEE_MAX_IE_SIZE +
		      le16_to_cpu(beacon_ie->ie_length);
		pos += len;
	}
	if (pr_ie && le16_to_cpu(pr_ie->ie_index) == MWIFIEX_AUTO_IDX_MASK) {
		/* save probe resp ie index after auto-indexing */
		*probe_idx = *((u16 *)pos);
		len = sizeof(*pr_ie) - IEEE_MAX_IE_SIZE +
		      le16_to_cpu(pr_ie->ie_length);
		pos += len;
	}
	if (ar_ie && le16_to_cpu(ar_ie->ie_index) == MWIFIEX_AUTO_IDX_MASK)
		/* save assoc resp ie index after auto-indexing */
		*assoc_idx = *((u16 *)pos);

	kfree(ap_custom_ie);
	return ret;
}