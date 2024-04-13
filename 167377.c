static void hso_serial_cleanup(struct tty_struct *tty)
{
	struct hso_serial *serial = tty->driver_data;

	if (!serial)
		return;

	kref_put(&serial->parent->ref, hso_serial_ref_free);
}