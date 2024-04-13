struct hso_shared_int *hso_create_shared_int(struct usb_interface *interface)
{
	struct hso_shared_int *mux = kzalloc(sizeof(*mux), GFP_KERNEL);

	if (!mux)
		return NULL;

	mux->intr_endp = hso_get_ep(interface, USB_ENDPOINT_XFER_INT,
				    USB_DIR_IN);
	if (!mux->intr_endp) {
		dev_err(&interface->dev, "Can't find INT IN endpoint\n");
		goto exit;
	}

	mux->shared_intr_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!mux->shared_intr_urb)
		goto exit;
	mux->shared_intr_buf =
		kzalloc(le16_to_cpu(mux->intr_endp->wMaxPacketSize),
			GFP_KERNEL);
	if (!mux->shared_intr_buf)
		goto exit;

	mutex_init(&mux->shared_int_lock);

	return mux;

exit:
	kfree(mux->shared_intr_buf);
	usb_free_urb(mux->shared_intr_urb);
	kfree(mux);
	return NULL;
}