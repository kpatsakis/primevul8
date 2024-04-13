static int _rtl_usb_init_rx(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_usb_priv *usb_priv = rtl_usbpriv(hw);
	struct rtl_usb *rtlusb = rtl_usbdev(usb_priv);

	rtlusb->rx_max_size = rtlpriv->cfg->usb_interface_cfg->rx_max_size;
	rtlusb->rx_urb_num = rtlpriv->cfg->usb_interface_cfg->rx_urb_num;
	rtlusb->in_ep = rtlpriv->cfg->usb_interface_cfg->in_ep_num;
	rtlusb->usb_rx_hdl = rtlpriv->cfg->usb_interface_cfg->usb_rx_hdl;
	rtlusb->usb_rx_segregate_hdl =
		rtlpriv->cfg->usb_interface_cfg->usb_rx_segregate_hdl;

	pr_info("rx_max_size %d, rx_urb_num %d, in_ep %d\n",
		rtlusb->rx_max_size, rtlusb->rx_urb_num, rtlusb->in_ep);
	init_usb_anchor(&rtlusb->rx_submitted);
	init_usb_anchor(&rtlusb->rx_cleanup_urbs);

	skb_queue_head_init(&rtlusb->rx_queue);
	rtlusb->rx_work_tasklet.func = _rtl_rx_work;
	rtlusb->rx_work_tasklet.data = (unsigned long)rtlusb;

	return 0;
}