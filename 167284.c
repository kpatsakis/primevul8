static struct urb *_rtl_usb_tx_urb_setup(struct ieee80211_hw *hw,
				struct sk_buff *skb, u32 ep_num)
{
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));
	struct urb *_urb;

	WARN_ON(NULL == skb);
	_urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!_urb) {
		kfree_skb(skb);
		return NULL;
	}
	_rtl_install_trx_info(rtlusb, skb, ep_num);
	usb_fill_bulk_urb(_urb, rtlusb->udev, usb_sndbulkpipe(rtlusb->udev,
			  ep_num), skb->data, skb->len, _rtl_tx_complete, skb);
	_urb->transfer_flags |= URB_ZERO_PACKET;
	return _urb;
}