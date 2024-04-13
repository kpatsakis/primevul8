void rtl_lps_set_psmode(struct ieee80211_hw *hw, u8 rt_psmode)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	bool enter_fwlps;

	if (mac->opmode == NL80211_IFTYPE_ADHOC)
		return;

	if (mac->link_state != MAC80211_LINKED)
		return;

	if (ppsc->dot11_psmode == rt_psmode && rt_psmode == EACTIVE)
		return;

	/* Update power save mode configured. */
	ppsc->dot11_psmode = rt_psmode;

	/*
	 *<FW control LPS>
	 *1. Enter PS mode
	 *   Set RPWM to Fw to turn RF off and send H2C fw_pwrmode
	 *   cmd to set Fw into PS mode.
	 *2. Leave PS mode
	 *   Send H2C fw_pwrmode cmd to Fw to set Fw into Active
	 *   mode and set RPWM to turn RF on.
	 */

	if ((ppsc->fwctrl_lps) && ppsc->report_linked) {
		if (ppsc->dot11_psmode == EACTIVE) {
			RT_TRACE(rtlpriv, COMP_RF, DBG_DMESG,
				 "FW LPS leave ps_mode:%x\n",
				  FW_PS_ACTIVE_MODE);
			enter_fwlps = false;
			ppsc->pwr_mode = FW_PS_ACTIVE_MODE;
			ppsc->smart_ps = 0;
			rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_FW_LPS_ACTION,
						      (u8 *)(&enter_fwlps));
			if (ppsc->p2p_ps_info.opp_ps)
				rtl_p2p_ps_cmd(hw , P2P_PS_ENABLE);

			if (rtlpriv->cfg->ops->get_btc_status())
				rtlpriv->btcoexist.btc_ops->btc_lps_notify(rtlpriv, rt_psmode);
		} else {
			if (rtl_get_fwlps_doze(hw)) {
				RT_TRACE(rtlpriv, COMP_RF, DBG_DMESG,
					 "FW LPS enter ps_mode:%x\n",
					 ppsc->fwctrl_psmode);
				if (rtlpriv->cfg->ops->get_btc_status())
					rtlpriv->btcoexist.btc_ops->btc_lps_notify(rtlpriv, rt_psmode);
				enter_fwlps = true;
				ppsc->pwr_mode = ppsc->fwctrl_psmode;
				ppsc->smart_ps = 2;
				rtlpriv->cfg->ops->set_hw_reg(hw,
							HW_VAR_FW_LPS_ACTION,
							(u8 *)(&enter_fwlps));

			} else {
				/* Reset the power save related parameters. */
				ppsc->dot11_psmode = EACTIVE;
			}
		}
	}
}