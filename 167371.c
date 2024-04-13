static void hso_serial_set_termios(struct tty_struct *tty, struct ktermios *old)
{
	struct hso_serial *serial = tty->driver_data;
	unsigned long flags;

	if (old)
		hso_dbg(0x16, "Termios called with: cflags new[%d] - old[%d]\n",
			tty->termios.c_cflag, old->c_cflag);

	/* the actual setup */
	spin_lock_irqsave(&serial->serial_lock, flags);
	if (serial->port.count)
		_hso_serial_set_termios(tty, old);
	else
		tty->termios = *old;
	spin_unlock_irqrestore(&serial->serial_lock, flags);

	/* done */
}