mwifiex_ie_get_autoidx(struct mwifiex_private *priv, u16 subtype_mask,
		       struct mwifiex_ie *ie, u16 *index)
{
	u16 mask, len, i;

	for (i = 0; i < priv->adapter->max_mgmt_ie_index; i++) {
		mask = le16_to_cpu(priv->mgmt_ie[i].mgmt_subtype_mask);
		len = le16_to_cpu(ie->ie_length);

		if (mask == MWIFIEX_AUTO_IDX_MASK)
			continue;

		if (mask == subtype_mask) {
			if (len > IEEE_MAX_IE_SIZE)
				continue;

			*index = i;
			return 0;
		}

		if (!priv->mgmt_ie[i].ie_length) {
			if (mwifiex_ie_index_used_by_other_intf(priv, i))
				continue;

			*index = i;
			return 0;
		}
	}

	return -1;
}