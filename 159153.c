static void _rtl_ps_inactive_ps(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));

	ppsc->swrf_processing = true;

	if (ppsc->inactive_pwrstate == ERFON &&
	    rtlhal->interface == INTF_PCI) {
		if ((ppsc->reg_rfps_level & RT_RF_OFF_LEVL_ASPM) &&
		    RT_IN_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM) &&
		    rtlhal->interface == INTF_PCI) {
			rtlpriv->intf_ops->disable_aspm(hw);
			RT_CLEAR_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM);
		}
	}

	rtl_ps_set_rf_state(hw, ppsc->inactive_pwrstate,
			    RF_CHANGE_BY_IPS);

	if (ppsc->inactive_pwrstate == ERFOFF &&
	    rtlhal->interface == INTF_PCI) {
		if (ppsc->reg_rfps_level & RT_RF_OFF_LEVL_ASPM &&
		    !RT_IN_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM)) {
			rtlpriv->intf_ops->enable_aspm(hw);
			RT_SET_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM);
		}
	}

	ppsc->swrf_processing = false;
}