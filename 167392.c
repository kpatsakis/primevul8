static struct hso_device *hso_create_bulk_serial_device(
			struct usb_interface *interface, int port)
{
	struct hso_device *hso_dev;
	struct hso_serial *serial;
	int num_urbs;
	struct hso_tiocmget *tiocmget;

	hso_dev = hso_create_device(interface, port);
	if (!hso_dev)
		return NULL;

	serial = kzalloc(sizeof(*serial), GFP_KERNEL);
	if (!serial)
		goto exit;

	serial->parent = hso_dev;
	hso_dev->port_data.dev_serial = serial;

	if ((port & HSO_PORT_MASK) == HSO_PORT_MODEM) {
		num_urbs = 2;
		serial->tiocmget = kzalloc(sizeof(struct hso_tiocmget),
					   GFP_KERNEL);
		/* it isn't going to break our heart if serial->tiocmget
		 *  allocation fails don't bother checking this.
		 */
		if (serial->tiocmget) {
			tiocmget = serial->tiocmget;
			tiocmget->urb = usb_alloc_urb(0, GFP_KERNEL);
			if (tiocmget->urb) {
				mutex_init(&tiocmget->mutex);
				init_waitqueue_head(&tiocmget->waitq);
				tiocmget->endp = hso_get_ep(
					interface,
					USB_ENDPOINT_XFER_INT,
					USB_DIR_IN);
			} else
				hso_free_tiomget(serial);
		}
	}
	else
		num_urbs = 1;

	if (hso_serial_common_create(serial, num_urbs, BULK_URB_RX_SIZE,
				     BULK_URB_TX_SIZE))
		goto exit;

	serial->in_endp = hso_get_ep(interface, USB_ENDPOINT_XFER_BULK,
				     USB_DIR_IN);
	if (!serial->in_endp) {
		dev_err(&interface->dev, "Failed to find BULK IN ep\n");
		goto exit2;
	}

	if (!
	    (serial->out_endp =
	     hso_get_ep(interface, USB_ENDPOINT_XFER_BULK, USB_DIR_OUT))) {
		dev_err(&interface->dev, "Failed to find BULK IN ep\n");
		goto exit2;
	}

	serial->write_data = hso_std_serial_write_data;

	/* and record this serial */
	set_serial_by_index(serial->minor, serial);

	/* setup the proc dirs and files if needed */
	hso_log_port(hso_dev);

	/* done, return it */
	return hso_dev;

exit2:
	hso_serial_tty_unregister(serial);
	hso_serial_common_free(serial);
exit:
	hso_free_tiomget(serial);
	kfree(serial);
	kfree(hso_dev);
	return NULL;
}