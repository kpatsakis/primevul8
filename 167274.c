static void _rtl_rx_pre_process(struct ieee80211_hw *hw, struct sk_buff *skb)
{
	struct sk_buff *_skb;
	struct sk_buff_head rx_queue;
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));

	skb_queue_head_init(&rx_queue);
	if (rtlusb->usb_rx_segregate_hdl)
		rtlusb->usb_rx_segregate_hdl(hw, skb, &rx_queue);
	WARN_ON(skb_queue_empty(&rx_queue));
	while (!skb_queue_empty(&rx_queue)) {
		_skb = skb_dequeue(&rx_queue);
		_rtl_usb_rx_process_agg(hw, _skb);
		ieee80211_rx(hw, _skb);
	}
}