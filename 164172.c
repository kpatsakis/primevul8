int mwifiex_del_mgmt_ies(struct mwifiex_private *priv)
{
	struct mwifiex_ie *beacon_ie = NULL, *pr_ie = NULL;
	struct mwifiex_ie *ar_ie = NULL, *gen_ie = NULL;
	int ret = 0;

	if (priv->gen_idx != MWIFIEX_AUTO_IDX_MASK) {
		gen_ie = kmalloc(sizeof(*gen_ie), GFP_KERNEL);
		if (!gen_ie)
			return -ENOMEM;

		gen_ie->ie_index = cpu_to_le16(priv->gen_idx);
		gen_ie->mgmt_subtype_mask = cpu_to_le16(MWIFIEX_DELETE_MASK);
		gen_ie->ie_length = 0;
		if (mwifiex_update_uap_custom_ie(priv, gen_ie, &priv->gen_idx,
						 NULL, &priv->proberesp_idx,
						 NULL, &priv->assocresp_idx)) {
			ret = -1;
			goto done;
		}

		priv->gen_idx = MWIFIEX_AUTO_IDX_MASK;
	}

	if (priv->beacon_idx != MWIFIEX_AUTO_IDX_MASK) {
		beacon_ie = kmalloc(sizeof(struct mwifiex_ie), GFP_KERNEL);
		if (!beacon_ie) {
			ret = -ENOMEM;
			goto done;
		}
		beacon_ie->ie_index = cpu_to_le16(priv->beacon_idx);
		beacon_ie->mgmt_subtype_mask = cpu_to_le16(MWIFIEX_DELETE_MASK);
		beacon_ie->ie_length = 0;
	}
	if (priv->proberesp_idx != MWIFIEX_AUTO_IDX_MASK) {
		pr_ie = kmalloc(sizeof(struct mwifiex_ie), GFP_KERNEL);
		if (!pr_ie) {
			ret = -ENOMEM;
			goto done;
		}
		pr_ie->ie_index = cpu_to_le16(priv->proberesp_idx);
		pr_ie->mgmt_subtype_mask = cpu_to_le16(MWIFIEX_DELETE_MASK);
		pr_ie->ie_length = 0;
	}
	if (priv->assocresp_idx != MWIFIEX_AUTO_IDX_MASK) {
		ar_ie = kmalloc(sizeof(struct mwifiex_ie), GFP_KERNEL);
		if (!ar_ie) {
			ret = -ENOMEM;
			goto done;
		}
		ar_ie->ie_index = cpu_to_le16(priv->assocresp_idx);
		ar_ie->mgmt_subtype_mask = cpu_to_le16(MWIFIEX_DELETE_MASK);
		ar_ie->ie_length = 0;
	}

	if (beacon_ie || pr_ie || ar_ie)
		ret = mwifiex_update_uap_custom_ie(priv,
						   beacon_ie, &priv->beacon_idx,
						   pr_ie, &priv->proberesp_idx,
						   ar_ie, &priv->assocresp_idx);

done:
	kfree(gen_ie);
	kfree(beacon_ie);
	kfree(pr_ie);
	kfree(ar_ie);

	return ret;
}