static void tiocmget_intr_callback(struct urb *urb)
{
	struct hso_serial *serial = urb->context;
	struct hso_tiocmget *tiocmget;
	int status = urb->status;
	u16 UART_state_bitmap, prev_UART_state_bitmap;
	struct uart_icount *icount;
	struct hso_serial_state_notification *serial_state_notification;
	struct usb_device *usb;
	struct usb_interface *interface;
	int if_num;

	/* Sanity checks */
	if (!serial)
		return;
	if (status) {
		handle_usb_error(status, __func__, serial->parent);
		return;
	}

	/* tiocmget is only supported on HSO_PORT_MODEM */
	tiocmget = serial->tiocmget;
	if (!tiocmget)
		return;
	BUG_ON((serial->parent->port_spec & HSO_PORT_MASK) != HSO_PORT_MODEM);

	usb = serial->parent->usb;
	interface = serial->parent->interface;

	if_num = interface->cur_altsetting->desc.bInterfaceNumber;

	/* wIndex should be the USB interface number of the port to which the
	 * notification applies, which should always be the Modem port.
	 */
	serial_state_notification = &tiocmget->serial_state_notification;
	if (serial_state_notification->bmRequestType != BM_REQUEST_TYPE ||
	    serial_state_notification->bNotification != B_NOTIFICATION ||
	    le16_to_cpu(serial_state_notification->wValue) != W_VALUE ||
	    le16_to_cpu(serial_state_notification->wIndex) != if_num ||
	    le16_to_cpu(serial_state_notification->wLength) != W_LENGTH) {
		dev_warn(&usb->dev,
			 "hso received invalid serial state notification\n");
		DUMP(serial_state_notification,
		     sizeof(struct hso_serial_state_notification));
	} else {
		unsigned long flags;

		UART_state_bitmap = le16_to_cpu(serial_state_notification->
						UART_state_bitmap);
		prev_UART_state_bitmap = tiocmget->prev_UART_state_bitmap;
		icount = &tiocmget->icount;
		spin_lock_irqsave(&serial->serial_lock, flags);
		if ((UART_state_bitmap & B_OVERRUN) !=
		   (prev_UART_state_bitmap & B_OVERRUN))
			icount->parity++;
		if ((UART_state_bitmap & B_PARITY) !=
		   (prev_UART_state_bitmap & B_PARITY))
			icount->parity++;
		if ((UART_state_bitmap & B_FRAMING) !=
		   (prev_UART_state_bitmap & B_FRAMING))
			icount->frame++;
		if ((UART_state_bitmap & B_RING_SIGNAL) &&
		   !(prev_UART_state_bitmap & B_RING_SIGNAL))
			icount->rng++;
		if ((UART_state_bitmap & B_BREAK) !=
		   (prev_UART_state_bitmap & B_BREAK))
			icount->brk++;
		if ((UART_state_bitmap & B_TX_CARRIER) !=
		   (prev_UART_state_bitmap & B_TX_CARRIER))
			icount->dsr++;
		if ((UART_state_bitmap & B_RX_CARRIER) !=
		   (prev_UART_state_bitmap & B_RX_CARRIER))
			icount->dcd++;
		tiocmget->prev_UART_state_bitmap = UART_state_bitmap;
		spin_unlock_irqrestore(&serial->serial_lock, flags);
		tiocmget->intr_completed = 1;
		wake_up_interruptible(&tiocmget->waitq);
	}
	memset(serial_state_notification, 0,
	       sizeof(struct hso_serial_state_notification));
	tiocmget_submit_urb(serial,
			    tiocmget,
			    serial->parent->usb);
}