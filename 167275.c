static int _rtl_usb_init(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_usb_priv *usb_priv = rtl_usbpriv(hw);
	struct rtl_usb *rtlusb = rtl_usbdev(usb_priv);
	int err;
	u8 epidx;
	struct usb_interface	*usb_intf = rtlusb->intf;
	u8 epnums = usb_intf->cur_altsetting->desc.bNumEndpoints;

	rtlusb->out_ep_nums = rtlusb->in_ep_nums = 0;
	for (epidx = 0; epidx < epnums; epidx++) {
		struct usb_endpoint_descriptor *pep_desc;

		pep_desc = &usb_intf->cur_altsetting->endpoint[epidx].desc;

		if (usb_endpoint_dir_in(pep_desc))
			rtlusb->in_ep_nums++;
		else if (usb_endpoint_dir_out(pep_desc))
			rtlusb->out_ep_nums++;

		RT_TRACE(rtlpriv, COMP_INIT, DBG_DMESG,
			 "USB EP(0x%02x), MaxPacketSize=%d, Interval=%d\n",
			 pep_desc->bEndpointAddress, pep_desc->wMaxPacketSize,
			 pep_desc->bInterval);
	}
	if (rtlusb->in_ep_nums <  rtlpriv->cfg->usb_interface_cfg->in_ep_num) {
		pr_err("Too few input end points found\n");
		return -EINVAL;
	}
	if (rtlusb->out_ep_nums == 0) {
		pr_err("No output end points found\n");
		return -EINVAL;
	}
	/* usb endpoint mapping */
	err = rtlpriv->cfg->usb_interface_cfg->usb_endpoint_mapping(hw);
	rtlusb->usb_mq_to_hwq =  rtlpriv->cfg->usb_interface_cfg->usb_mq_to_hwq;
	_rtl_usb_init_tx(hw);
	_rtl_usb_init_rx(hw);
	return err;
}