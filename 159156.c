void rtl_p2p_info(struct ieee80211_hw *hw, void *data, unsigned int len)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct ieee80211_hdr *hdr = data;

	if (!mac->p2p)
		return;
	if (mac->link_state != MAC80211_LINKED)
		return;
	/* min. beacon length + FCS_LEN */
	if (len <= 40 + FCS_LEN)
		return;

	/* and only beacons from the associated BSSID, please */
	if (!ether_addr_equal_64bits(hdr->addr3, rtlpriv->mac80211.bssid))
		return;

	/* check if this really is a beacon */
	if (!(ieee80211_is_beacon(hdr->frame_control) ||
	      ieee80211_is_probe_resp(hdr->frame_control) ||
	      ieee80211_is_action(hdr->frame_control)))
		return;

	if (ieee80211_is_action(hdr->frame_control))
		rtl_p2p_action_ie(hw , data , len - FCS_LEN);
	else
		rtl_p2p_noa_ie(hw , data , len - FCS_LEN);
}