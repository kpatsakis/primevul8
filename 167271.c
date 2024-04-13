static int rtl_usb_tx(struct ieee80211_hw *hw,
		      struct ieee80211_sta *sta,
		      struct sk_buff *skb,
		      struct rtl_tcb_desc *dummy)
{
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)(skb->data);
	__le16 fc = hdr->frame_control;
	u16 hw_queue;

	if (unlikely(is_hal_stop(rtlhal)))
		goto err_free;
	hw_queue = rtlusb->usb_mq_to_hwq(fc, skb_get_queue_mapping(skb));
	_rtl_usb_tx_preprocess(hw, sta, skb, hw_queue);
	_rtl_usb_transmit(hw, skb, hw_queue);
	return NETDEV_TX_OK;

err_free:
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}