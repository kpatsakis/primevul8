static void _rtl_submit_tx_urb(struct ieee80211_hw *hw, struct urb *_urb)
{
	int err;
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));

	usb_anchor_urb(_urb, &rtlusb->tx_submitted);
	err = usb_submit_urb(_urb, GFP_ATOMIC);
	if (err < 0) {
		struct sk_buff *skb;

		pr_err("Failed to submit urb\n");
		usb_unanchor_urb(_urb);
		skb = (struct sk_buff *)_urb->context;
		kfree_skb(skb);
	}
	usb_free_urb(_urb);
}