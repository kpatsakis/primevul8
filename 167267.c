static void rtl_fill_h2c_cmd_work_callback(struct work_struct *work)
{
	struct rtl_works *rtlworks =
	    container_of(work, struct rtl_works, fill_h2c_cmd);
	struct ieee80211_hw *hw = rtlworks->hw;
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtlpriv->cfg->ops->fill_h2c_cmd(hw, H2C_RA_MASK, 5, rtlpriv->rate_mask);
}