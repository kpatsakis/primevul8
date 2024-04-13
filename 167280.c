static void _rtl_tx_complete(struct urb *urb)
{
	struct sk_buff *skb = (struct sk_buff *)urb->context;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct rtl_usb *rtlusb = (struct rtl_usb *)info->rate_driver_data[0];
	struct ieee80211_hw *hw = usb_get_intfdata(rtlusb->intf);
	int err;

	if (unlikely(IS_USB_STOP(rtlusb)))
		return;
	err = _usb_tx_post(hw, urb, skb);
	if (err) {
		/* Ignore error and keep issuiing other urbs */
		return;
	}
}