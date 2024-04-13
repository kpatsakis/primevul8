void rtl_swlps_rf_awake(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));

	if (!rtlpriv->psc.swctrl_lps)
		return;
	if (mac->link_state != MAC80211_LINKED)
		return;

	if (ppsc->reg_rfps_level & RT_RF_LPS_LEVEL_ASPM &&
	    RT_IN_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM)) {
		rtlpriv->intf_ops->disable_aspm(hw);
		RT_CLEAR_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM);
	}

	mutex_lock(&rtlpriv->locks.lps_mutex);
	rtl_ps_set_rf_state(hw, ERFON, RF_CHANGE_BY_PS);
	mutex_unlock(&rtlpriv->locks.lps_mutex);
}