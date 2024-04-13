static void rtl_usb_cleanup(struct ieee80211_hw *hw)
{
	u32 i;
	struct sk_buff *_skb;
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));
	struct ieee80211_tx_info *txinfo;

	/* clean up rx stuff. */
	_rtl_usb_cleanup_rx(hw);

	/* clean up tx stuff */
	for (i = 0; i < RTL_USB_MAX_EP_NUM; i++) {
		while ((_skb = skb_dequeue(&rtlusb->tx_skb_queue[i]))) {
			rtlusb->usb_tx_cleanup(hw, _skb);
			txinfo = IEEE80211_SKB_CB(_skb);
			ieee80211_tx_info_clear_status(txinfo);
			txinfo->flags |= IEEE80211_TX_STAT_ACK;
			ieee80211_tx_status_irqsafe(hw, _skb);
		}
		usb_kill_anchored_urbs(&rtlusb->tx_pending[i]);
	}
	usb_kill_anchored_urbs(&rtlusb->tx_submitted);
}