void mwifiex_config_uap_11d(struct mwifiex_private *priv,
			    struct cfg80211_beacon_data *beacon_data)
{
	enum state_11d_t state_11d;
	const u8 *country_ie;

	country_ie = cfg80211_find_ie(WLAN_EID_COUNTRY, beacon_data->tail,
				      beacon_data->tail_len);
	if (country_ie) {
		/* Send cmd to FW to enable 11D function */
		state_11d = ENABLE_11D;
		if (mwifiex_send_cmd(priv, HostCmd_CMD_802_11_SNMP_MIB,
				     HostCmd_ACT_GEN_SET, DOT11D_I,
				     &state_11d, true)) {
			mwifiex_dbg(priv->adapter, ERROR,
				    "11D: failed to enable 11D\n");
		}
	}
}