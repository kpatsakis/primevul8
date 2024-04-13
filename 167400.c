static void hso_serial_close(struct tty_struct *tty, struct file *filp)
{
	struct hso_serial *serial = tty->driver_data;
	u8 usb_gone;

	hso_dbg(0x1, "Closing serial port\n");

	/* Open failed, no close cleanup required */
	if (serial == NULL)
		return;

	mutex_lock(&serial->parent->mutex);
	usb_gone = serial->parent->usb_gone;

	if (!usb_gone)
		usb_autopm_get_interface(serial->parent->interface);

	/* reset the rts and dtr */
	/* do the actual close */
	serial->port.count--;

	if (serial->port.count <= 0) {
		serial->port.count = 0;
		tty_port_tty_set(&serial->port, NULL);
		if (!usb_gone)
			hso_stop_serial_device(serial->parent);
		tasklet_kill(&serial->unthrottle_tasklet);
	}

	if (!usb_gone)
		usb_autopm_put_interface(serial->parent->interface);

	mutex_unlock(&serial->parent->mutex);
}