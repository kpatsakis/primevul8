int mwifiex_set_mgmt_ies(struct mwifiex_private *priv,
			 struct cfg80211_beacon_data *info)
{
	int ret;

	ret = mwifiex_uap_parse_tail_ies(priv, info);

	if (ret)
		return ret;

	return mwifiex_set_mgmt_beacon_data_ies(priv, info);
}