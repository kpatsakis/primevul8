static void put_rxbuf_data_and_resubmit_bulk_urb(struct hso_serial *serial)
{
	int count;
	struct urb *curr_urb;

	while (serial->rx_urb_filled[serial->curr_rx_urb_idx]) {
		curr_urb = serial->rx_urb[serial->curr_rx_urb_idx];
		count = put_rxbuf_data(curr_urb, serial);
		if (count == -1)
			return;
		if (count == 0) {
			serial->curr_rx_urb_idx++;
			if (serial->curr_rx_urb_idx >= serial->num_rx_urbs)
				serial->curr_rx_urb_idx = 0;
			hso_resubmit_rx_bulk_urb(serial, curr_urb);
		}
	}
}