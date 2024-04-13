static void ctrl_callback(struct urb *urb)
{
	struct hso_serial *serial = urb->context;
	struct usb_ctrlrequest *req;
	int status = urb->status;
	unsigned long flags;

	/* sanity check */
	if (!serial)
		return;

	spin_lock_irqsave(&serial->serial_lock, flags);
	serial->tx_urb_used = 0;
	spin_unlock_irqrestore(&serial->serial_lock, flags);
	if (status) {
		handle_usb_error(status, __func__, serial->parent);
		return;
	}

	/* what request? */
	req = (struct usb_ctrlrequest *)(urb->setup_packet);
	hso_dbg(0x8, "--- Got muxed ctrl callback 0x%02X ---\n", status);
	hso_dbg(0x8, "Actual length of urb = %d\n", urb->actual_length);
	DUMP1(urb->transfer_buffer, urb->actual_length);

	if (req->bRequestType ==
	    (USB_DIR_IN | USB_TYPE_OPTION_VENDOR | USB_RECIP_INTERFACE)) {
		/* response to a read command */
		serial->rx_urb_filled[0] = 1;
		spin_lock_irqsave(&serial->serial_lock, flags);
		put_rxbuf_data_and_resubmit_ctrl_urb(serial);
		spin_unlock_irqrestore(&serial->serial_lock, flags);
	} else {
		hso_put_activity(serial->parent);
		tty_port_tty_wakeup(&serial->port);
		/* response to a write command */
		hso_kick_transmit(serial);
	}
}