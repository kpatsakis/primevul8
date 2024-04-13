mwifiex_update_autoindex_ies(struct mwifiex_private *priv,
			     struct mwifiex_ie_list *ie_list)
{
	u16 travel_len, index, mask;
	s16 input_len, tlv_len;
	struct mwifiex_ie *ie;
	u8 *tmp;

	input_len = le16_to_cpu(ie_list->len);
	travel_len = sizeof(struct mwifiex_ie_types_header);

	ie_list->len = 0;

	while (input_len >= sizeof(struct mwifiex_ie_types_header)) {
		ie = (struct mwifiex_ie *)(((u8 *)ie_list) + travel_len);
		tlv_len = le16_to_cpu(ie->ie_length);
		travel_len += tlv_len + MWIFIEX_IE_HDR_SIZE;

		if (input_len < tlv_len + MWIFIEX_IE_HDR_SIZE)
			return -1;
		index = le16_to_cpu(ie->ie_index);
		mask = le16_to_cpu(ie->mgmt_subtype_mask);

		if (index == MWIFIEX_AUTO_IDX_MASK) {
			/* automatic addition */
			if (mwifiex_ie_get_autoidx(priv, mask, ie, &index))
				return -1;
			if (index == MWIFIEX_AUTO_IDX_MASK)
				return -1;

			tmp = (u8 *)&priv->mgmt_ie[index].ie_buffer;
			memcpy(tmp, &ie->ie_buffer, le16_to_cpu(ie->ie_length));
			priv->mgmt_ie[index].ie_length = ie->ie_length;
			priv->mgmt_ie[index].ie_index = cpu_to_le16(index);
			priv->mgmt_ie[index].mgmt_subtype_mask =
							cpu_to_le16(mask);

			ie->ie_index = cpu_to_le16(index);
		} else {
			if (mask != MWIFIEX_DELETE_MASK)
				return -1;
			/*
			 * Check if this index is being used on any
			 * other interface.
			 */
			if (mwifiex_ie_index_used_by_other_intf(priv, index))
				return -1;

			ie->ie_length = 0;
			memcpy(&priv->mgmt_ie[index], ie,
			       sizeof(struct mwifiex_ie));
		}

		le16_unaligned_add_cpu(&ie_list->len,
				       le16_to_cpu(
					    priv->mgmt_ie[index].ie_length) +
				       MWIFIEX_IE_HDR_SIZE);
		input_len -= tlv_len + MWIFIEX_IE_HDR_SIZE;
	}

	if (GET_BSS_ROLE(priv) == MWIFIEX_BSS_ROLE_UAP)
		return mwifiex_send_cmd(priv, HostCmd_CMD_UAP_SYS_CONFIG,
					HostCmd_ACT_GEN_SET,
					UAP_CUSTOM_IE_I, ie_list, true);

	return 0;
}