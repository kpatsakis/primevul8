static int hso_serial_open(struct tty_struct *tty, struct file *filp)
{
	struct hso_serial *serial = get_serial_by_index(tty->index);
	int result;

	/* sanity check */
	if (serial == NULL || serial->magic != HSO_SERIAL_MAGIC) {
		WARN_ON(1);
		tty->driver_data = NULL;
		hso_dbg(0x1, "Failed to open port\n");
		return -ENODEV;
	}

	mutex_lock(&serial->parent->mutex);
	result = usb_autopm_get_interface(serial->parent->interface);
	if (result < 0)
		goto err_out;

	hso_dbg(0x1, "Opening %d\n", serial->minor);

	/* setup */
	tty->driver_data = serial;
	tty_port_tty_set(&serial->port, tty);

	/* check for port already opened, if not set the termios */
	serial->port.count++;
	if (serial->port.count == 1) {
		serial->rx_state = RX_IDLE;
		/* Force default termio settings */
		_hso_serial_set_termios(tty, NULL);
		tasklet_init(&serial->unthrottle_tasklet,
			     (void (*)(unsigned long))hso_unthrottle_tasklet,
			     (unsigned long)serial);
		result = hso_start_serial_device(serial->parent, GFP_KERNEL);
		if (result) {
			hso_stop_serial_device(serial->parent);
			serial->port.count--;
		} else {
			kref_get(&serial->parent->ref);
		}
	} else {
		hso_dbg(0x1, "Port was already open\n");
	}

	usb_autopm_put_interface(serial->parent->interface);

	/* done */
	if (result)
		hso_serial_tiocmset(tty, TIOCM_RTS | TIOCM_DTR, 0);
err_out:
	mutex_unlock(&serial->parent->mutex);
	return result;
}