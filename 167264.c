static void _rtl_rx_completed(struct urb *_urb)
{
	struct rtl_usb *rtlusb = (struct rtl_usb *)_urb->context;
	int err = 0;

	if (unlikely(IS_USB_STOP(rtlusb)))
		goto free;

	if (likely(0 == _urb->status)) {
		unsigned int padding;
		struct sk_buff *skb;
		unsigned int qlen;
		unsigned int size = _urb->actual_length;
		struct ieee80211_hdr *hdr;

		if (size < RTL_RX_DESC_SIZE + sizeof(struct ieee80211_hdr)) {
			pr_err("Too short packet from bulk IN! (len: %d)\n",
			       size);
			goto resubmit;
		}

		qlen = skb_queue_len(&rtlusb->rx_queue);
		if (qlen >= __RX_SKB_MAX_QUEUED) {
			pr_err("Pending RX skbuff queue full! (qlen: %d)\n",
			       qlen);
			goto resubmit;
		}

		hdr = (void *)(_urb->transfer_buffer + RTL_RX_DESC_SIZE);
		padding = _rtl_rx_get_padding(hdr, size - RTL_RX_DESC_SIZE);

		skb = dev_alloc_skb(size + __RADIO_TAP_SIZE_RSV + padding);
		if (!skb) {
			pr_err("Can't allocate skb for bulk IN!\n");
			goto resubmit;
		}

		_rtl_install_trx_info(rtlusb, skb, rtlusb->in_ep);

		/* Make sure the payload data is 4 byte aligned. */
		skb_reserve(skb, padding);

		/* reserve some space for mac80211's radiotap */
		skb_reserve(skb, __RADIO_TAP_SIZE_RSV);

		skb_put_data(skb, _urb->transfer_buffer, size);

		skb_queue_tail(&rtlusb->rx_queue, skb);
		tasklet_schedule(&rtlusb->rx_work_tasklet);

		goto resubmit;
	}

	switch (_urb->status) {
	/* disconnect */
	case -ENOENT:
	case -ECONNRESET:
	case -ENODEV:
	case -ESHUTDOWN:
		goto free;
	default:
		break;
	}

resubmit:
	usb_anchor_urb(_urb, &rtlusb->rx_submitted);
	err = usb_submit_urb(_urb, GFP_ATOMIC);
	if (unlikely(err)) {
		usb_unanchor_urb(_urb);
		goto free;
	}
	return;

free:
	/* On some architectures, usb_free_coherent must not be called from
	 * hardirq context. Queue urb to cleanup list.
	 */
	usb_anchor_urb(_urb, &rtlusb->rx_cleanup_urbs);
}