static int hso_serial_common_create(struct hso_serial *serial, int num_urbs,
				    int rx_size, int tx_size)
{
	int minor;
	int i;

	tty_port_init(&serial->port);

	minor = get_free_serial_index();
	if (minor < 0)
		goto exit;

	/* register our minor number */
	serial->parent->dev = tty_port_register_device_attr(&serial->port,
			tty_drv, minor, &serial->parent->interface->dev,
			serial->parent, hso_serial_dev_groups);

	/* fill in specific data for later use */
	serial->minor = minor;
	serial->magic = HSO_SERIAL_MAGIC;
	spin_lock_init(&serial->serial_lock);
	serial->num_rx_urbs = num_urbs;

	/* RX, allocate urb and initialize */

	/* prepare our RX buffer */
	serial->rx_data_length = rx_size;
	for (i = 0; i < serial->num_rx_urbs; i++) {
		serial->rx_urb[i] = usb_alloc_urb(0, GFP_KERNEL);
		if (!serial->rx_urb[i])
			goto exit;
		serial->rx_urb[i]->transfer_buffer = NULL;
		serial->rx_urb[i]->transfer_buffer_length = 0;
		serial->rx_data[i] = kzalloc(serial->rx_data_length,
					     GFP_KERNEL);
		if (!serial->rx_data[i])
			goto exit;
	}

	/* TX, allocate urb and initialize */
	serial->tx_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!serial->tx_urb)
		goto exit;
	serial->tx_urb->transfer_buffer = NULL;
	serial->tx_urb->transfer_buffer_length = 0;
	/* prepare our TX buffer */
	serial->tx_data_count = 0;
	serial->tx_buffer_count = 0;
	serial->tx_data_length = tx_size;
	serial->tx_data = kzalloc(serial->tx_data_length, GFP_KERNEL);
	if (!serial->tx_data)
		goto exit;

	serial->tx_buffer = kzalloc(serial->tx_data_length, GFP_KERNEL);
	if (!serial->tx_buffer)
		goto exit;

	return 0;
exit:
	hso_serial_tty_unregister(serial);
	hso_serial_common_free(serial);
	return -1;
}