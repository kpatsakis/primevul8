static void hso_unthrottle_tasklet(struct hso_serial *serial)
{
	unsigned long flags;

	spin_lock_irqsave(&serial->serial_lock, flags);
	if ((serial->parent->port_spec & HSO_INTF_MUX))
		put_rxbuf_data_and_resubmit_ctrl_urb(serial);
	else
		put_rxbuf_data_and_resubmit_bulk_urb(serial);
	spin_unlock_irqrestore(&serial->serial_lock, flags);
}