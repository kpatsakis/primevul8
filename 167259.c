static int _rtl_usb_init_tx(struct ieee80211_hw *hw)
{
	u32 i;
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));

	rtlusb->max_bulk_out_size = IS_HIGH_SPEED_USB(rtlusb->udev)
						    ? USB_HIGH_SPEED_BULK_SIZE
						    : USB_FULL_SPEED_BULK_SIZE;

	RT_TRACE(rtlpriv, COMP_INIT, DBG_DMESG, "USB Max Bulk-out Size=%d\n",
		 rtlusb->max_bulk_out_size);

	for (i = 0; i < __RTL_TXQ_NUM; i++) {
		u32 ep_num = rtlusb->ep_map.ep_mapping[i];

		if (!ep_num) {
			RT_TRACE(rtlpriv, COMP_INIT, DBG_DMESG,
				 "Invalid endpoint map setting!\n");
			return -EINVAL;
		}
	}

	rtlusb->usb_tx_post_hdl =
		 rtlpriv->cfg->usb_interface_cfg->usb_tx_post_hdl;
	rtlusb->usb_tx_cleanup	=
		 rtlpriv->cfg->usb_interface_cfg->usb_tx_cleanup;
	rtlusb->usb_tx_aggregate_hdl =
		 (rtlpriv->cfg->usb_interface_cfg->usb_tx_aggregate_hdl)
		 ? rtlpriv->cfg->usb_interface_cfg->usb_tx_aggregate_hdl
		 : &_none_usb_tx_aggregate_hdl;

	init_usb_anchor(&rtlusb->tx_submitted);
	for (i = 0; i < RTL_USB_MAX_EP_NUM; i++) {
		skb_queue_head_init(&rtlusb->tx_skb_queue[i]);
		init_usb_anchor(&rtlusb->tx_pending[i]);
	}
	return 0;
}