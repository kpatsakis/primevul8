static int hso_start_serial_device(struct hso_device *hso_dev, gfp_t flags)
{
	int i, result = 0;
	struct hso_serial *serial = dev2ser(hso_dev);

	if (!serial)
		return -ENODEV;

	/* If it is not the MUX port fill in and submit a bulk urb (already
	 * allocated in hso_serial_start) */
	if (!(serial->parent->port_spec & HSO_INTF_MUX)) {
		for (i = 0; i < serial->num_rx_urbs; i++) {
			usb_fill_bulk_urb(serial->rx_urb[i],
					  serial->parent->usb,
					  usb_rcvbulkpipe(serial->parent->usb,
							  serial->in_endp->
							  bEndpointAddress &
							  0x7F),
					  serial->rx_data[i],
					  serial->rx_data_length,
					  hso_std_serial_read_bulk_callback,
					  serial);
			result = usb_submit_urb(serial->rx_urb[i], flags);
			if (result) {
				dev_warn(&serial->parent->usb->dev,
					 "Failed to submit urb - res %d\n",
					 result);
				break;
			}
		}
	} else {
		mutex_lock(&serial->shared_int->shared_int_lock);
		if (!serial->shared_int->use_count) {
			result =
			    hso_mux_submit_intr_urb(serial->shared_int,
						    hso_dev->usb, flags);
		}
		serial->shared_int->use_count++;
		mutex_unlock(&serial->shared_int->shared_int_lock);
	}
	if (serial->tiocmget)
		tiocmget_submit_urb(serial,
				    serial->tiocmget,
				    serial->parent->usb);
	return result;
}