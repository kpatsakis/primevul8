static int _usb_tx_post(struct ieee80211_hw *hw, struct urb *urb,
			struct sk_buff *skb)
{
	struct rtl_usb *rtlusb = rtl_usbdev(rtl_usbpriv(hw));
	struct ieee80211_tx_info *txinfo;

	rtlusb->usb_tx_post_hdl(hw, urb, skb);
	skb_pull(skb, RTL_TX_HEADER_SIZE);
	txinfo = IEEE80211_SKB_CB(skb);
	ieee80211_tx_info_clear_status(txinfo);
	txinfo->flags |= IEEE80211_TX_STAT_ACK;

	if (urb->status) {
		pr_err("Urb has error status 0x%X\n", urb->status);
		goto out;
	}
	/*  TODO:	statistics */
out:
	ieee80211_tx_status_irqsafe(hw, skb);
	return urb->status;
}