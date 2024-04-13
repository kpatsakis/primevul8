mwifiex_ie_index_used_by_other_intf(struct mwifiex_private *priv, u16 idx)
{
	int i;
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_ie *ie;

	for (i = 0; i < adapter->priv_num; i++) {
		if (adapter->priv[i] != priv) {
			ie = &adapter->priv[i]->mgmt_ie[idx];
			if (ie->mgmt_subtype_mask && ie->ie_length)
				return -1;
		}
	}

	return 0;
}