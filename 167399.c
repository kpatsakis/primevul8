static int hso_serial_chars_in_buffer(struct tty_struct *tty)
{
	struct hso_serial *serial = tty->driver_data;
	int chars;
	unsigned long flags;

	/* sanity check */
	if (serial == NULL)
		return 0;

	spin_lock_irqsave(&serial->serial_lock, flags);
	chars = serial->tx_buffer_count;
	spin_unlock_irqrestore(&serial->serial_lock, flags);

	return chars;
}