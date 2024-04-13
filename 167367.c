static int hso_stop_serial_device(struct hso_device *hso_dev)
{
	int i;
	struct hso_serial *serial = dev2ser(hso_dev);
	struct hso_tiocmget  *tiocmget;

	if (!serial)
		return -ENODEV;

	for (i = 0; i < serial->num_rx_urbs; i++) {
		if (serial->rx_urb[i]) {
			usb_kill_urb(serial->rx_urb[i]);
			serial->rx_urb_filled[i] = 0;
		}
	}
	serial->curr_rx_urb_idx = 0;

	if (serial->tx_urb)
		usb_kill_urb(serial->tx_urb);

	if (serial->shared_int) {
		mutex_lock(&serial->shared_int->shared_int_lock);
		if (serial->shared_int->use_count &&
		    (--serial->shared_int->use_count == 0)) {
			struct urb *urb;

			urb = serial->shared_int->shared_intr_urb;
			if (urb)
				usb_kill_urb(urb);
		}
		mutex_unlock(&serial->shared_int->shared_int_lock);
	}
	tiocmget = serial->tiocmget;
	if (tiocmget) {
		wake_up_interruptible(&tiocmget->waitq);
		usb_kill_urb(tiocmget->urb);
	}

	return 0;
}