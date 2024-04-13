static void _rtl_usb_cleanup_rx(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));
	struct urb *urb;

	usb_kill_anchored_urbs(&rtlusb->rx_submitted);

	tasklet_kill(&rtlusb->rx_work_tasklet);
	cancel_work_sync(&rtlpriv->works.lps_change_work);

	flush_workqueue(rtlpriv->works.rtl_wq);
	destroy_workqueue(rtlpriv->works.rtl_wq);

	skb_queue_purge(&rtlusb->rx_queue);

	while ((urb = usb_get_from_anchor(&rtlusb->rx_cleanup_urbs))) {
		usb_free_coherent(urb->dev, urb->transfer_buffer_length,
				urb->transfer_buffer, urb->transfer_dma);
		usb_free_urb(urb);
	}
}