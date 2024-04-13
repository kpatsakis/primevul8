static int hso_get_count(struct tty_struct *tty,
		  struct serial_icounter_struct *icount)
{
	struct uart_icount cnow;
	struct hso_serial *serial = tty->driver_data;
	struct hso_tiocmget  *tiocmget = serial->tiocmget;

	memset(icount, 0, sizeof(struct serial_icounter_struct));

	if (!tiocmget)
		 return -ENOENT;
	spin_lock_irq(&serial->serial_lock);
	memcpy(&cnow, &tiocmget->icount, sizeof(struct uart_icount));
	spin_unlock_irq(&serial->serial_lock);

	icount->cts         = cnow.cts;
	icount->dsr         = cnow.dsr;
	icount->rng         = cnow.rng;
	icount->dcd         = cnow.dcd;
	icount->rx          = cnow.rx;
	icount->tx          = cnow.tx;
	icount->frame       = cnow.frame;
	icount->overrun     = cnow.overrun;
	icount->parity      = cnow.parity;
	icount->brk         = cnow.brk;
	icount->buf_overrun = cnow.buf_overrun;

	return 0;
}