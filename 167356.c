static void hso_std_serial_write_bulk_callback(struct urb *urb)
{
	struct hso_serial *serial = urb->context;
	int status = urb->status;
	unsigned long flags;

	/* sanity check */
	if (!serial) {
		hso_dbg(0x1, "serial == NULL\n");
		return;
	}

	spin_lock_irqsave(&serial->serial_lock, flags);
	serial->tx_urb_used = 0;
	spin_unlock_irqrestore(&serial->serial_lock, flags);
	if (status) {
		handle_usb_error(status, __func__, serial->parent);
		return;
	}
	hso_put_activity(serial->parent);
	tty_port_tty_wakeup(&serial->port);
	hso_kick_transmit(serial);

	hso_dbg(0x1, "\n");
}