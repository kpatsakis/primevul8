static void hso_init_termios(struct ktermios *termios)
{
	/*
	 * The default requirements for this device are:
	 */
	termios->c_iflag &=
		~(IGNBRK	/* disable ignore break */
		| BRKINT	/* disable break causes interrupt */
		| PARMRK	/* disable mark parity errors */
		| ISTRIP	/* disable clear high bit of input characters */
		| INLCR		/* disable translate NL to CR */
		| IGNCR		/* disable ignore CR */
		| ICRNL		/* disable translate CR to NL */
		| IXON);	/* disable enable XON/XOFF flow control */

	/* disable postprocess output characters */
	termios->c_oflag &= ~OPOST;

	termios->c_lflag &=
		~(ECHO		/* disable echo input characters */
		| ECHONL	/* disable echo new line */
		| ICANON	/* disable erase, kill, werase, and rprnt
				   special characters */
		| ISIG		/* disable interrupt, quit, and suspend special
				   characters */
		| IEXTEN);	/* disable non-POSIX special characters */

	termios->c_cflag &=
		~(CSIZE		/* no size */
		| PARENB	/* disable parity bit */
		| CBAUD		/* clear current baud rate */
		| CBAUDEX);	/* clear current buad rate */

	termios->c_cflag |= CS8;	/* character size 8 bits */

	/* baud rate 115200 */
	tty_termios_encode_baud_rate(termios, 115200, 115200);
}