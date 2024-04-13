static struct sk_buff *_none_usb_tx_aggregate_hdl(struct ieee80211_hw *hw,
						  struct sk_buff_head *list)
{
	return skb_dequeue(list);
}