static int _rtl_usb_receive(struct ieee80211_hw *hw)
{
	struct urb *urb;
	int err;
	int i;
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));

	WARN_ON(0 == rtlusb->rx_urb_num);
	/* 1600 == 1514 + max WLAN header + rtk info */
	WARN_ON(rtlusb->rx_max_size < 1600);

	for (i = 0; i < rtlusb->rx_urb_num; i++) {
		err = -ENOMEM;
		urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!urb)
			goto err_out;

		err = _rtl_prep_rx_urb(hw, rtlusb, urb, GFP_KERNEL);
		if (err < 0) {
			pr_err("Failed to prep_rx_urb!!\n");
			usb_free_urb(urb);
			goto err_out;
		}

		usb_anchor_urb(urb, &rtlusb->rx_submitted);
		err = usb_submit_urb(urb, GFP_KERNEL);
		if (err)
			goto err_out;
		usb_free_urb(urb);
	}
	return 0;

err_out:
	usb_kill_anchored_urbs(&rtlusb->rx_submitted);
	_rtl_usb_cleanup_rx(hw);
	return err;
}