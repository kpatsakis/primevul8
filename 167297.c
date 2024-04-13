static int _rtl_prep_rx_urb(struct ieee80211_hw *hw, struct rtl_usb *rtlusb,
			      struct urb *urb, gfp_t gfp_mask)
{
	void *buf;

	buf = usb_alloc_coherent(rtlusb->udev, rtlusb->rx_max_size, gfp_mask,
				 &urb->transfer_dma);
	if (!buf) {
		pr_err("Failed to usb_alloc_coherent!!\n");
		return -ENOMEM;
	}

	usb_fill_bulk_urb(urb, rtlusb->udev,
			  usb_rcvbulkpipe(rtlusb->udev, rtlusb->in_ep),
			  buf, rtlusb->rx_max_size, _rtl_rx_completed, rtlusb);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	return 0;
}