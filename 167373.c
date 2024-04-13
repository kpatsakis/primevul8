static void hso_free_interface(struct usb_interface *interface)
{
	struct hso_serial *serial;
	int i;

	for (i = 0; i < HSO_SERIAL_TTY_MINORS; i++) {
		if (serial_table[i] &&
		    (serial_table[i]->interface == interface)) {
			serial = dev2ser(serial_table[i]);
			tty_port_tty_hangup(&serial->port, false);
			mutex_lock(&serial->parent->mutex);
			serial->parent->usb_gone = 1;
			mutex_unlock(&serial->parent->mutex);
			cancel_work_sync(&serial_table[i]->async_put_intf);
			cancel_work_sync(&serial_table[i]->async_get_intf);
			hso_serial_tty_unregister(serial);
			kref_put(&serial_table[i]->ref, hso_serial_ref_free);
			set_serial_by_index(i, NULL);
		}
	}

	for (i = 0; i < HSO_MAX_NET_DEVICES; i++) {
		if (network_table[i] &&
		    (network_table[i]->interface == interface)) {
			struct rfkill *rfk = dev2net(network_table[i])->rfkill;
			/* hso_stop_net_device doesn't stop the net queue since
			 * traffic needs to start it again when suspended */
			netif_stop_queue(dev2net(network_table[i])->net);
			hso_stop_net_device(network_table[i]);
			cancel_work_sync(&network_table[i]->async_put_intf);
			cancel_work_sync(&network_table[i]->async_get_intf);
			if (rfk) {
				rfkill_unregister(rfk);
				rfkill_destroy(rfk);
			}
			hso_free_net_device(network_table[i]);
		}
	}
}