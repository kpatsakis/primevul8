static void hso_resubmit_rx_bulk_urb(struct hso_serial *serial, struct urb *urb)
{
	int result;
	/* We are done with this URB, resubmit it. Prep the USB to wait for
	 * another frame */
	usb_fill_bulk_urb(urb, serial->parent->usb,
			  usb_rcvbulkpipe(serial->parent->usb,
					  serial->in_endp->
					  bEndpointAddress & 0x7F),
			  urb->transfer_buffer, serial->rx_data_length,
			  hso_std_serial_read_bulk_callback, serial);
	/* Give this to the USB subsystem so it can tell us when more data
	 * arrives. */
	result = usb_submit_urb(urb, GFP_ATOMIC);
	if (result) {
		dev_err(&urb->dev->dev, "%s failed submit serial rx_urb %d\n",
			__func__, result);
	}
}