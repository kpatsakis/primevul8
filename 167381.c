static void hso_serial_tty_unregister(struct hso_serial *serial)
{
	tty_unregister_device(tty_drv, serial->minor);
}