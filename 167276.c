static int rtl_usb_start(struct ieee80211_hw *hw)
{
	int err;
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));

	err = rtlpriv->cfg->ops->hw_init(hw);
	if (!err) {
		rtl_init_rx_config(hw);

		/* Enable software */
		SET_USB_START(rtlusb);
		/* should after adapter start and interrupt enable. */
		set_hal_start(rtlhal);

		/* Start bulk IN */
		err = _rtl_usb_receive(hw);
	}

	return err;
}