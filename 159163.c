void rtl_swlps_wq_callback(void *data)
{
	struct rtl_works *rtlworks = container_of_dwork_rtl(data,
				     struct rtl_works,
				     ps_work);
	struct ieee80211_hw *hw = rtlworks->hw;
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	bool ps = false;

	ps = (hw->conf.flags & IEEE80211_CONF_PS);

	/* we can sleep after ps null send ok */
	if (rtlpriv->psc.state_inap) {
		rtl_swlps_rf_sleep(hw);

		if (rtlpriv->psc.state && !ps) {
			rtlpriv->psc.sleep_ms = jiffies_to_msecs(jiffies -
						 rtlpriv->psc.last_action);
		}

		if (ps)
			rtlpriv->psc.last_slept = jiffies;

		rtlpriv->psc.last_action = jiffies;
		rtlpriv->psc.state = ps;
	}
}