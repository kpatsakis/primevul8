static void hso_std_serial_read_bulk_callback(struct urb *urb)
{
	struct hso_serial *serial = urb->context;
	int status = urb->status;
	unsigned long flags;

	hso_dbg(0x8, "--- Got serial_read_bulk callback %02x ---\n", status);

	/* sanity check */
	if (!serial) {
		hso_dbg(0x1, "serial == NULL\n");
		return;
	}
	if (status) {
		handle_usb_error(status, __func__, serial->parent);
		return;
	}

	hso_dbg(0x1, "Actual length = %d\n", urb->actual_length);
	DUMP1(urb->transfer_buffer, urb->actual_length);

	/* Anyone listening? */
	if (serial->port.count == 0)
		return;

	if (serial->parent->port_spec & HSO_INFO_CRC_BUG)
		fix_crc_bug(urb, serial->in_endp->wMaxPacketSize);
	/* Valid data, handle RX data */
	spin_lock_irqsave(&serial->serial_lock, flags);
	serial->rx_urb_filled[hso_urb_to_index(serial, urb)] = 1;
	put_rxbuf_data_and_resubmit_bulk_urb(serial);
	spin_unlock_irqrestore(&serial->serial_lock, flags);
}