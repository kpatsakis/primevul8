static void _hso_serial_set_termios(struct tty_struct *tty,
				    struct ktermios *old)
{
	struct hso_serial *serial = tty->driver_data;

	if (!serial) {
		pr_err("%s: no tty structures", __func__);
		return;
	}

	hso_dbg(0x8, "port %d\n", serial->minor);

	/*
	 *	Fix up unsupported bits
	 */
	tty->termios.c_iflag &= ~IXON; /* disable enable XON/XOFF flow control */

	tty->termios.c_cflag &=
		~(CSIZE		/* no size */
		| PARENB	/* disable parity bit */
		| CBAUD		/* clear current baud rate */
		| CBAUDEX);	/* clear current buad rate */

	tty->termios.c_cflag |= CS8;	/* character size 8 bits */

	/* baud rate 115200 */
	tty_encode_baud_rate(tty, 115200, 115200);
}