static void rtl_usb_init_sw(struct ieee80211_hw *hw)
{
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));

	rtlhal->hw = hw;
	ppsc->inactiveps = false;
	ppsc->leisure_ps = false;
	ppsc->fwctrl_lps = false;
	ppsc->reg_fwctrl_lps = 3;
	ppsc->reg_max_lps_awakeintvl = 5;
	ppsc->fwctrl_psmode = FW_PS_DTIM_MODE;

	 /* IBSS */
	mac->beacon_interval = 100;

	 /* AMPDU */
	mac->min_space_cfg = 0;
	mac->max_mss_density = 0;

	/* set sane AMPDU defaults */
	mac->current_ampdu_density = 7;
	mac->current_ampdu_factor = 3;

	/* QOS */
	rtlusb->acm_method = EACMWAY2_SW;

	/* IRQ */
	/* HIMR - turn all on */
	rtlusb->irq_mask[0] = 0xFFFFFFFF;
	/* HIMR_EX - turn all on */
	rtlusb->irq_mask[1] = 0xFFFFFFFF;
	rtlusb->disablehwsm =  true;
}