static int hso_serial_write_room(struct tty_struct *tty)
{
	struct hso_serial *serial = tty->driver_data;
	int room;
	unsigned long flags;

	spin_lock_irqsave(&serial->serial_lock, flags);
	room = serial->tx_data_length - serial->tx_buffer_count;
	spin_unlock_irqrestore(&serial->serial_lock, flags);

	/* return free room */
	return room;
}