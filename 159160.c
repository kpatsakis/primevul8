bool rtl_ps_enable_nic(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_mac *rtlmac = rtl_mac(rtl_priv(hw));

	/*<1> reset trx ring */
	if (rtlhal->interface == INTF_PCI)
		rtlpriv->intf_ops->reset_trx_ring(hw);

	if (is_hal_stop(rtlhal))
		RT_TRACE(rtlpriv, COMP_ERR, DBG_WARNING,
			 "Driver is already down!\n");

	/*<2> Enable Adapter */
	if (rtlpriv->cfg->ops->hw_init(hw))
		return false;
	rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_RETRY_LIMIT,
			&rtlmac->retry_long);
	RT_CLEAR_PS_LEVEL(ppsc, RT_RF_OFF_LEVL_HALT_NIC);

	rtlpriv->cfg->ops->switch_channel(hw);
	rtlpriv->cfg->ops->set_channel_access(hw);
	rtlpriv->cfg->ops->set_bw_mode(hw,
			cfg80211_get_chandef_type(&hw->conf.chandef));

	/*<3> Enable Interrupt */
	rtlpriv->cfg->ops->enable_interrupt(hw);

	/*<enable timer> */
	rtl_watch_dog_timer_callback(&rtlpriv->works.watchdog_timer);

	return true;
}