static void rtl_lps_leave_core(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));

	mutex_lock(&rtlpriv->locks.lps_mutex);

	if (ppsc->fwctrl_lps) {
		if (ppsc->dot11_psmode != EACTIVE) {

			/*FIX ME */
			/*rtlpriv->cfg->ops->enable_interrupt(hw); */

			if (ppsc->reg_rfps_level & RT_RF_LPS_LEVEL_ASPM &&
			    RT_IN_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM) &&
			    rtlhal->interface == INTF_PCI) {
				rtlpriv->intf_ops->disable_aspm(hw);
				RT_CLEAR_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM);
			}

			RT_TRACE(rtlpriv, COMP_POWER, DBG_LOUD,
				 "Busy Traffic,Leave 802.11 power save..\n");

			rtl_lps_set_psmode(hw, EACTIVE);
		}
	}
	mutex_unlock(&rtlpriv->locks.lps_mutex);
}