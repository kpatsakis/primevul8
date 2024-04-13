static void intr_callback(struct urb *urb)
{
	struct hso_shared_int *shared_int = urb->context;
	struct hso_serial *serial;
	unsigned char *port_req;
	int status = urb->status;
	unsigned long flags;
	int i;

	usb_mark_last_busy(urb->dev);

	/* sanity check */
	if (!shared_int)
		return;

	/* status check */
	if (status) {
		handle_usb_error(status, __func__, NULL);
		return;
	}
	hso_dbg(0x8, "--- Got intr callback 0x%02X ---\n", status);

	/* what request? */
	port_req = urb->transfer_buffer;
	hso_dbg(0x8, "port_req = 0x%.2X\n", *port_req);
	/* loop over all muxed ports to find the one sending this */
	for (i = 0; i < 8; i++) {
		/* max 8 channels on MUX */
		if (*port_req & (1 << i)) {
			serial = get_serial_by_shared_int_and_type(shared_int,
								   (1 << i));
			if (serial != NULL) {
				hso_dbg(0x1, "Pending read interrupt on port %d\n",
					i);
				spin_lock_irqsave(&serial->serial_lock, flags);
				if (serial->rx_state == RX_IDLE &&
					serial->port.count > 0) {
					/* Setup and send a ctrl req read on
					 * port i */
					if (!serial->rx_urb_filled[0]) {
						serial->rx_state = RX_SENT;
						hso_mux_serial_read(serial);
					} else
						serial->rx_state = RX_PENDING;
				} else {
					hso_dbg(0x1, "Already a read pending on port %d or port not open\n",
						i);
				}
				spin_unlock_irqrestore(&serial->serial_lock,
						       flags);
			}
		}
	}
	/* Resubmit interrupt urb */
	hso_mux_submit_intr_urb(shared_int, urb->dev, GFP_ATOMIC);
}