void rtl_swlps_rfon_wq_callback(void *data)
{
	struct rtl_works *rtlworks =
	    container_of_dwork_rtl(data, struct rtl_works, ps_rfon_wq);
	struct ieee80211_hw *hw = rtlworks->hw;

	rtl_swlps_rf_awake(hw);
}