static int hso_std_serial_write_data(struct hso_serial *serial)
{
	int count = serial->tx_data_count;
	int result;

	usb_fill_bulk_urb(serial->tx_urb,
			  serial->parent->usb,
			  usb_sndbulkpipe(serial->parent->usb,
					  serial->out_endp->
					  bEndpointAddress & 0x7F),
			  serial->tx_data, serial->tx_data_count,
			  hso_std_serial_write_bulk_callback, serial);

	result = usb_submit_urb(serial->tx_urb, GFP_ATOMIC);
	if (result) {
		dev_warn(&serial->parent->usb->dev,
			 "Failed to submit urb - res %d\n", result);
		return result;
	}

	return count;
}