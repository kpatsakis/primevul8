struct hso_device *hso_create_mux_serial_device(struct usb_interface *interface,
						int port,
						struct hso_shared_int *mux)
{
	struct hso_device *hso_dev;
	struct hso_serial *serial;
	int port_spec;

	port_spec = HSO_INTF_MUX;
	port_spec &= ~HSO_PORT_MASK;

	port_spec |= hso_mux_to_port(port);
	if ((port_spec & HSO_PORT_MASK) == HSO_PORT_NO_PORT)
		return NULL;

	hso_dev = hso_create_device(interface, port_spec);
	if (!hso_dev)
		return NULL;

	serial = kzalloc(sizeof(*serial), GFP_KERNEL);
	if (!serial)
		goto exit;

	hso_dev->port_data.dev_serial = serial;
	serial->parent = hso_dev;

	if (hso_serial_common_create
	    (serial, 1, CTRL_URB_RX_SIZE, CTRL_URB_TX_SIZE))
		goto exit;

	serial->tx_data_length--;
	serial->write_data = hso_mux_serial_write_data;

	serial->shared_int = mux;
	mutex_lock(&serial->shared_int->shared_int_lock);
	serial->shared_int->ref_count++;
	mutex_unlock(&serial->shared_int->shared_int_lock);

	/* and record this serial */
	set_serial_by_index(serial->minor, serial);

	/* setup the proc dirs and files if needed */
	hso_log_port(hso_dev);

	/* done, return it */
	return hso_dev;

exit:
	if (serial) {
		tty_unregister_device(tty_drv, serial->minor);
		kfree(serial);
	}
	kfree(hso_dev);
	return NULL;

}