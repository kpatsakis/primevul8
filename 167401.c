static int hso_resume(struct usb_interface *iface)
{
	int i, result = 0;
	struct hso_net *hso_net;

	/* Start all serial ports */
	for (i = 0; i < HSO_SERIAL_TTY_MINORS; i++) {
		if (serial_table[i] && (serial_table[i]->interface == iface)) {
			if (dev2ser(serial_table[i])->port.count) {
				result =
				    hso_start_serial_device(serial_table[i], GFP_NOIO);
				hso_kick_transmit(dev2ser(serial_table[i]));
				if (result)
					goto out;
			}
		}
	}

	/* Start all network ports */
	for (i = 0; i < HSO_MAX_NET_DEVICES; i++) {
		if (network_table[i] &&
		    (network_table[i]->interface == iface)) {
			hso_net = dev2net(network_table[i]);
			if (hso_net->flags & IFF_UP) {
				/* First transmit any lingering data,
				   then restart the device. */
				if (hso_net->skb_tx_buf) {
					dev_dbg(&iface->dev,
						"Transmitting"
						" lingering data\n");
					hso_net_start_xmit(hso_net->skb_tx_buf,
							   hso_net->net);
					hso_net->skb_tx_buf = NULL;
				}
				result = hso_start_net_device(network_table[i]);
				if (result)
					goto out;
			}
		}
	}

out:
	return result;
}