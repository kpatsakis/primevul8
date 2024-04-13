static int hso_serial_tiocmget(struct tty_struct *tty)
{
	int retval;
	struct hso_serial *serial = tty->driver_data;
	struct hso_tiocmget  *tiocmget;
	u16 UART_state_bitmap;

	/* sanity check */
	if (!serial) {
		hso_dbg(0x1, "no tty structures\n");
		return -EINVAL;
	}
	spin_lock_irq(&serial->serial_lock);
	retval = ((serial->rts_state) ? TIOCM_RTS : 0) |
	    ((serial->dtr_state) ? TIOCM_DTR : 0);
	tiocmget = serial->tiocmget;
	if (tiocmget) {

		UART_state_bitmap = le16_to_cpu(
			tiocmget->prev_UART_state_bitmap);
		if (UART_state_bitmap & B_RING_SIGNAL)
			retval |=  TIOCM_RNG;
		if (UART_state_bitmap & B_RX_CARRIER)
			retval |=  TIOCM_CD;
		if (UART_state_bitmap & B_TX_CARRIER)
			retval |=  TIOCM_DSR;
	}
	spin_unlock_irq(&serial->serial_lock);
	return retval;
}