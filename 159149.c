void rtl_lps_change_work_callback(struct work_struct *work)
{
	struct rtl_works *rtlworks =
	    container_of(work, struct rtl_works, lps_change_work);
	struct ieee80211_hw *hw = rtlworks->hw;
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	if (rtlpriv->enter_ps)
		rtl_lps_enter_core(hw);
	else
		rtl_lps_leave_core(hw);
}