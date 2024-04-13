static int hso_urb_to_index(struct hso_serial *serial, struct urb *urb)
{
	int idx;

	for (idx = 0; idx < serial->num_rx_urbs; idx++)
		if (serial->rx_urb[idx] == urb)
			return idx;
	dev_err(serial->parent->dev, "hso_urb_to_index failed\n");
	return -1;
}