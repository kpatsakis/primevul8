bool rtl_ps_disable_nic(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	/*<1> Stop all timer */
	rtl_deinit_deferred_work(hw, true);

	/*<2> Disable Interrupt */
	rtlpriv->cfg->ops->disable_interrupt(hw);
	tasklet_kill(&rtlpriv->works.irq_tasklet);

	/*<3> Disable Adapter */
	rtlpriv->cfg->ops->hw_disable(hw);

	return true;
}