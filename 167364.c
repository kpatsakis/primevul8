static int hso_serial_ioctl(struct tty_struct *tty,
			    unsigned int cmd, unsigned long arg)
{
	struct hso_serial *serial = tty->driver_data;
	int ret = 0;
	hso_dbg(0x8, "IOCTL cmd: %d, arg: %ld\n", cmd, arg);

	if (!serial)
		return -ENODEV;
	switch (cmd) {
	case TIOCMIWAIT:
		ret = hso_wait_modem_status(serial, arg);
		break;
	default:
		ret = -ENOIOCTLCMD;
		break;
	}
	return ret;
}