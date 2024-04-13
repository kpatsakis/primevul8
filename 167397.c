static int hso_serial_write(struct tty_struct *tty, const unsigned char *buf,
			    int count)
{
	struct hso_serial *serial = tty->driver_data;
	int space, tx_bytes;
	unsigned long flags;

	/* sanity check */
	if (serial == NULL) {
		pr_err("%s: serial is NULL\n", __func__);
		return -ENODEV;
	}

	spin_lock_irqsave(&serial->serial_lock, flags);

	space = serial->tx_data_length - serial->tx_buffer_count;
	tx_bytes = (count < space) ? count : space;

	if (!tx_bytes)
		goto out;

	memcpy(serial->tx_buffer + serial->tx_buffer_count, buf, tx_bytes);
	serial->tx_buffer_count += tx_bytes;

out:
	spin_unlock_irqrestore(&serial->serial_lock, flags);

	hso_kick_transmit(serial);
	/* done */
	return tx_bytes;
}