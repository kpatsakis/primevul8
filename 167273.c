void rtl_usb_disconnect(struct usb_interface *intf)
{
	struct ieee80211_hw *hw = usb_get_intfdata(intf);
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *rtlmac = rtl_mac(rtl_priv(hw));
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));

	if (unlikely(!rtlpriv))
		return;
	/* just in case driver is removed before firmware callback */
	wait_for_completion(&rtlpriv->firmware_loading_complete);
	clear_bit(RTL_STATUS_INTERFACE_START, &rtlpriv->status);
	/*ieee80211_unregister_hw will call ops_stop */
	if (rtlmac->mac80211_registered == 1) {
		ieee80211_unregister_hw(hw);
		rtlmac->mac80211_registered = 0;
	} else {
		rtl_deinit_deferred_work(hw, false);
		rtlpriv->intf_ops->adapter_stop(hw);
	}
	/*deinit rfkill */
	/* rtl_deinit_rfkill(hw); */
	rtl_usb_deinit(hw);
	rtl_deinit_core(hw);
	kfree(rtlpriv->usb_data);
	rtlpriv->cfg->ops->deinit_sw_leds(hw);
	rtlpriv->cfg->ops->deinit_sw_vars(hw);
	_rtl_usb_io_handler_release(hw);
	usb_put_dev(rtlusb->udev);
	usb_set_intfdata(intf, NULL);
	ieee80211_free_hw(hw);
}