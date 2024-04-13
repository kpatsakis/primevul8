static int hso_mux_serial_write_data(struct hso_serial *serial)
{
	if (NULL == serial)
		return -EINVAL;

	return mux_device_request(serial,
				  USB_CDC_SEND_ENCAPSULATED_COMMAND,
				  serial->parent->port_spec & HSO_PORT_MASK,
				  serial->tx_urb,
				  &serial->ctrl_req_tx,
				  serial->tx_data, serial->tx_data_count);
}