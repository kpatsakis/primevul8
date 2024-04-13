static bool rtl_get_fwlps_doze(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	u32 ps_timediff;

	ps_timediff = jiffies_to_msecs(jiffies -
				       ppsc->last_delaylps_stamp_jiffies);

	if (ps_timediff < 2000) {
		RT_TRACE(rtlpriv, COMP_POWER, DBG_LOUD,
			 "Delay enter Fw LPS for DHCP, ARP, or EAPOL exchanging state\n");
		return false;
	}

	if (mac->link_state != MAC80211_LINKED)
		return false;

	if (mac->opmode == NL80211_IFTYPE_ADHOC)
		return false;

	return true;
}