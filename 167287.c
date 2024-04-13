static void rtl_usb_stop(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));
	struct urb *urb;

	/* should after adapter start and interrupt enable. */
	set_hal_stop(rtlhal);
	cancel_work_sync(&rtlpriv->works.fill_h2c_cmd);
	/* Enable software */
	SET_USB_STOP(rtlusb);

	/* free pre-allocated URBs from rtl_usb_start() */
	usb_kill_anchored_urbs(&rtlusb->rx_submitted);

	tasklet_kill(&rtlusb->rx_work_tasklet);
	cancel_work_sync(&rtlpriv->works.lps_change_work);

	flush_workqueue(rtlpriv->works.rtl_wq);

	skb_queue_purge(&rtlusb->rx_queue);

	while ((urb = usb_get_from_anchor(&rtlusb->rx_cleanup_urbs))) {
		usb_free_coherent(urb->dev, urb->transfer_buffer_length,
				urb->transfer_buffer, urb->transfer_dma);
		usb_free_urb(urb);
	}

	rtlpriv->cfg->ops->hw_disable(hw);
}