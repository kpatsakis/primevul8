static int hso_mux_submit_intr_urb(struct hso_shared_int *shared_int,
				   struct usb_device *usb, gfp_t gfp)
{
	int result;

	usb_fill_int_urb(shared_int->shared_intr_urb, usb,
			 usb_rcvintpipe(usb,
				shared_int->intr_endp->bEndpointAddress & 0x7F),
			 shared_int->shared_intr_buf,
			 1,
			 intr_callback, shared_int,
			 shared_int->intr_endp->bInterval);

	result = usb_submit_urb(shared_int->shared_intr_urb, gfp);
	if (result)
		dev_warn(&usb->dev, "%s failed mux_intr_urb %d\n", __func__,
			result);

	return result;
}