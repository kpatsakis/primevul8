void rtl_lps_enter(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	if (!in_interrupt())
		return rtl_lps_enter_core(hw);
	rtlpriv->enter_ps = true;
	schedule_work(&rtlpriv->works.lps_change_work);
}