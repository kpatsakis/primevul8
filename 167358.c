static int hso_serial_tiocmset(struct tty_struct *tty,
			       unsigned int set, unsigned int clear)
{
	int val = 0;
	unsigned long flags;
	int if_num;
	struct hso_serial *serial = tty->driver_data;
	struct usb_interface *interface;

	/* sanity check */
	if (!serial) {
		hso_dbg(0x1, "no tty structures\n");
		return -EINVAL;
	}

	if ((serial->parent->port_spec & HSO_PORT_MASK) != HSO_PORT_MODEM)
		return -EINVAL;

	interface = serial->parent->interface;
	if_num = interface->cur_altsetting->desc.bInterfaceNumber;

	spin_lock_irqsave(&serial->serial_lock, flags);
	if (set & TIOCM_RTS)
		serial->rts_state = 1;
	if (set & TIOCM_DTR)
		serial->dtr_state = 1;

	if (clear & TIOCM_RTS)
		serial->rts_state = 0;
	if (clear & TIOCM_DTR)
		serial->dtr_state = 0;

	if (serial->dtr_state)
		val |= 0x01;
	if (serial->rts_state)
		val |= 0x02;

	spin_unlock_irqrestore(&serial->serial_lock, flags);

	return usb_control_msg(serial->parent->usb,
			       usb_rcvctrlpipe(serial->parent->usb, 0), 0x22,
			       0x21, val, if_num, NULL, 0,
			       USB_CTRL_SET_TIMEOUT);
}