static void hso_serial_common_free(struct hso_serial *serial)
{
	int i;

	for (i = 0; i < serial->num_rx_urbs; i++) {
		/* unlink and free RX URB */
		usb_free_urb(serial->rx_urb[i]);
		/* free the RX buffer */
		kfree(serial->rx_data[i]);
	}

	/* unlink and free TX URB */
	usb_free_urb(serial->tx_urb);
	kfree(serial->tx_buffer);
	kfree(serial->tx_data);
	tty_port_destroy(&serial->port);
}