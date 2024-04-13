static int hso_mux_serial_read(struct hso_serial *serial)
{
	if (!serial)
		return -EINVAL;

	/* clean data */
	memset(serial->rx_data[0], 0, CTRL_URB_RX_SIZE);
	/* make the request */

	if (serial->num_rx_urbs != 1) {
		dev_err(&serial->parent->interface->dev,
			"ERROR: mux'd reads with multiple buffers "
			"not possible\n");
		return 0;
	}
	return mux_device_request(serial,
				  USB_CDC_GET_ENCAPSULATED_RESPONSE,
				  serial->parent->port_spec & HSO_PORT_MASK,
				  serial->rx_urb[0],
				  &serial->ctrl_req_rx,
				  serial->rx_data[0], serial->rx_data_length);
}