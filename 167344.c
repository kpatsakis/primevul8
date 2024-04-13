static int put_rxbuf_data(struct urb *urb, struct hso_serial *serial)
{
	struct tty_struct *tty;
	int count;

	/* Sanity check */
	if (urb == NULL || serial == NULL) {
		hso_dbg(0x1, "serial = NULL\n");
		return -2;
	}

	tty = tty_port_tty_get(&serial->port);

	if (tty && tty_throttled(tty)) {
		tty_kref_put(tty);
		return -1;
	}

	/* Push data to tty */
	hso_dbg(0x1, "data to push to tty\n");
	count = tty_buffer_request_room(&serial->port, urb->actual_length);
	if (count >= urb->actual_length) {
		tty_insert_flip_string(&serial->port, urb->transfer_buffer,
				       urb->actual_length);
		tty_flip_buffer_push(&serial->port);
	} else {
		dev_warn(&serial->parent->usb->dev,
			 "dropping data, %d bytes lost\n", urb->actual_length);
	}

	tty_kref_put(tty);

	serial->rx_urb_filled[hso_urb_to_index(serial, urb)] = 0;

	return 0;
}