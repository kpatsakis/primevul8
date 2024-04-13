static void _rtl_usb_tx_preprocess(struct ieee80211_hw *hw,
				   struct ieee80211_sta *sta,
				   struct sk_buff *skb,
				   u16 hw_queue)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct rtl_tx_desc *pdesc = NULL;
	struct rtl_tcb_desc tcb_desc;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)(skb->data);
	__le16 fc = hdr->frame_control;
	u8 *pda_addr = hdr->addr1;

	memset(&tcb_desc, 0, sizeof(struct rtl_tcb_desc));
	if (ieee80211_is_auth(fc)) {
		RT_TRACE(rtlpriv, COMP_SEND, DBG_DMESG, "MAC80211_LINKING\n");
	}

	if (rtlpriv->psc.sw_ps_enabled) {
		if (ieee80211_is_data(fc) && !ieee80211_is_nullfunc(fc) &&
		    !ieee80211_has_pm(fc))
			hdr->frame_control |= cpu_to_le16(IEEE80211_FCTL_PM);
	}

	rtl_action_proc(hw, skb, true);
	if (is_multicast_ether_addr(pda_addr))
		rtlpriv->stats.txbytesmulticast += skb->len;
	else if (is_broadcast_ether_addr(pda_addr))
		rtlpriv->stats.txbytesbroadcast += skb->len;
	else
		rtlpriv->stats.txbytesunicast += skb->len;
	rtlpriv->cfg->ops->fill_tx_desc(hw, hdr, (u8 *)pdesc, NULL, info, sta, skb,
					hw_queue, &tcb_desc);
	if (ieee80211_is_data(fc))
		rtlpriv->cfg->ops->led_control(hw, LED_CTL_TX);
}