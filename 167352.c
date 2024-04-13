static void put_rxbuf_data_and_resubmit_ctrl_urb(struct hso_serial *serial)
{
	int count = 0;
	struct urb *urb;

	urb = serial->rx_urb[0];
	if (serial->port.count > 0) {
		count = put_rxbuf_data(urb, serial);
		if (count == -1)
			return;
	}
	/* Re issue a read as long as we receive data. */

	if (count == 0 && ((urb->actual_length != 0) ||
			   (serial->rx_state == RX_PENDING))) {
		serial->rx_state = RX_SENT;
		hso_mux_serial_read(serial);
	} else
		serial->rx_state = RX_IDLE;
}