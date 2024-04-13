void rtl_swlps_rf_sleep(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	u8 sleep_intv;

	if (!rtlpriv->psc.sw_ps_enabled)
		return;

	if ((rtlpriv->sec.being_setkey) ||
	    (mac->opmode == NL80211_IFTYPE_ADHOC))
		return;

	/*sleep after linked 10s, to let DHCP and 4-way handshake ok enough!! */
	if ((mac->link_state != MAC80211_LINKED) || (mac->cnt_after_linked < 5))
		return;

	if (rtlpriv->link_info.busytraffic)
		return;

	spin_lock(&rtlpriv->locks.rf_ps_lock);
	if (rtlpriv->psc.rfchange_inprogress) {
		spin_unlock(&rtlpriv->locks.rf_ps_lock);
		return;
	}
	spin_unlock(&rtlpriv->locks.rf_ps_lock);

	mutex_lock(&rtlpriv->locks.lps_mutex);
	rtl_ps_set_rf_state(hw, ERFSLEEP, RF_CHANGE_BY_PS);
	mutex_unlock(&rtlpriv->locks.lps_mutex);

	if (ppsc->reg_rfps_level & RT_RF_OFF_LEVL_ASPM &&
	    !RT_IN_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM)) {
		rtlpriv->intf_ops->enable_aspm(hw);
		RT_SET_PS_LEVEL(ppsc, RT_PS_LEVEL_ASPM);
	}

	/* here is power save alg, when this beacon is DTIM
	 * we will set sleep time to dtim_period * n;
	 * when this beacon is not DTIM, we will set sleep
	 * time to sleep_intv = rtlpriv->psc.dtim_counter or
	 * MAX_SW_LPS_SLEEP_INTV(default set to 5) */

	if (rtlpriv->psc.dtim_counter == 0) {
		if (hw->conf.ps_dtim_period == 1)
			sleep_intv = hw->conf.ps_dtim_period * 2;
		else
			sleep_intv = hw->conf.ps_dtim_period;
	} else {
		sleep_intv = rtlpriv->psc.dtim_counter;
	}

	if (sleep_intv > MAX_SW_LPS_SLEEP_INTV)
		sleep_intv = MAX_SW_LPS_SLEEP_INTV;

	/* this print should always be dtim_conter = 0 &
	 * sleep  = dtim_period, that meaons, we should
	 * awake before every dtim */
	RT_TRACE(rtlpriv, COMP_POWER, DBG_DMESG,
		 "dtim_counter:%x will sleep :%d beacon_intv\n",
		  rtlpriv->psc.dtim_counter, sleep_intv);

	/* we tested that 40ms is enough for sw & hw sw delay */
	queue_delayed_work(rtlpriv->works.rtl_wq, &rtlpriv->works.ps_rfon_wq,
			MSECS(sleep_intv * mac->vif->bss_conf.beacon_int - 40));
}