static void hso_free_tiomget(struct hso_serial *serial)
{
	struct hso_tiocmget *tiocmget;
	if (!serial)
		return;
	tiocmget = serial->tiocmget;
	if (tiocmget) {
		usb_free_urb(tiocmget->urb);
		tiocmget->urb = NULL;
		serial->tiocmget = NULL;
		kfree(tiocmget);
	}
}