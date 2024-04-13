static void _rtl_usb_transmit(struct ieee80211_hw *hw, struct sk_buff *skb,
		       enum rtl_txq qnum)
{
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));
	u32 ep_num;
	struct urb *_urb = NULL;
	struct sk_buff *_skb = NULL;

	WARN_ON(NULL == rtlusb->usb_tx_aggregate_hdl);
	if (unlikely(IS_USB_STOP(rtlusb))) {
		pr_err("USB device is stopping...\n");
		kfree_skb(skb);
		return;
	}
	ep_num = rtlusb->ep_map.ep_mapping[qnum];
	_skb = skb;
	_urb = _rtl_usb_tx_urb_setup(hw, _skb, ep_num);
	if (unlikely(!_urb)) {
		pr_err("Can't allocate urb. Drop skb!\n");
		kfree_skb(skb);
		return;
	}
	_rtl_submit_tx_urb(hw, _urb);
}