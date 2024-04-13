static int hso_suspend(struct usb_interface *iface, pm_message_t message)
{
	int i, result;

	/* Stop all serial ports */
	for (i = 0; i < HSO_SERIAL_TTY_MINORS; i++) {
		if (serial_table[i] && (serial_table[i]->interface == iface)) {
			result = hso_stop_serial_device(serial_table[i]);
			if (result)
				goto out;
		}
	}

	/* Stop all network ports */
	for (i = 0; i < HSO_MAX_NET_DEVICES; i++) {
		if (network_table[i] &&
		    (network_table[i]->interface == iface)) {
			result = hso_stop_net_device(network_table[i]);
			if (result)
				goto out;
		}
	}

out:
	return 0;
}