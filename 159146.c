void rtl_ips_nic_off(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	/* because when link with ap, mac80211 will ask us
	 * to disable nic quickly after scan before linking,
	 * this will cause link failed, so we delay 100ms here
	 */
	queue_delayed_work(rtlpriv->works.rtl_wq,
			   &rtlpriv->works.ips_nic_off_wq, MSECS(100));
}