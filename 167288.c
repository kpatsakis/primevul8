static void _rtl_usb_rx_process_agg(struct ieee80211_hw *hw,
				    struct sk_buff *skb)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	u8 *rxdesc = skb->data;
	struct ieee80211_hdr *hdr;
	bool unicast = false;
	__le16 fc;
	struct ieee80211_rx_status rx_status = {0};
	struct rtl_stats stats = {
		.signal = 0,
		.rate = 0,
	};

	skb_pull(skb, RTL_RX_DESC_SIZE);
	rtlpriv->cfg->ops->query_rx_desc(hw, &stats, &rx_status, rxdesc, skb);
	skb_pull(skb, (stats.rx_drvinfo_size + stats.rx_bufshift));
	hdr = (struct ieee80211_hdr *)(skb->data);
	fc = hdr->frame_control;
	if (!stats.crc) {
		memcpy(IEEE80211_SKB_RXCB(skb), &rx_status, sizeof(rx_status));

		if (is_broadcast_ether_addr(hdr->addr1)) {
			/*TODO*/;
		} else if (is_multicast_ether_addr(hdr->addr1)) {
			/*TODO*/
		} else {
			unicast = true;
			rtlpriv->stats.rxbytesunicast +=  skb->len;
		}

		if (ieee80211_is_data(fc)) {
			rtlpriv->cfg->ops->led_control(hw, LED_CTL_RX);

			if (unicast)
				rtlpriv->link_info.num_rx_inperiod++;
		}
		/* static bcn for roaming */
		rtl_beacon_statistic(hw, skb);
	}
}