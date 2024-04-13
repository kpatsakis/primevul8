hso_wait_modem_status(struct hso_serial *serial, unsigned long arg)
{
	DECLARE_WAITQUEUE(wait, current);
	struct uart_icount cprev, cnow;
	struct hso_tiocmget  *tiocmget;
	int ret;

	tiocmget = serial->tiocmget;
	if (!tiocmget)
		return -ENOENT;
	/*
	 * note the counters on entry
	 */
	spin_lock_irq(&serial->serial_lock);
	memcpy(&cprev, &tiocmget->icount, sizeof(struct uart_icount));
	spin_unlock_irq(&serial->serial_lock);
	add_wait_queue(&tiocmget->waitq, &wait);
	for (;;) {
		spin_lock_irq(&serial->serial_lock);
		memcpy(&cnow, &tiocmget->icount, sizeof(struct uart_icount));
		spin_unlock_irq(&serial->serial_lock);
		set_current_state(TASK_INTERRUPTIBLE);
		if (((arg & TIOCM_RNG) && (cnow.rng != cprev.rng)) ||
		    ((arg & TIOCM_DSR) && (cnow.dsr != cprev.dsr)) ||
		    ((arg & TIOCM_CD)  && (cnow.dcd != cprev.dcd))) {
			ret = 0;
			break;
		}
		schedule();
		/* see if a signal did it */
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			break;
		}
		cprev = cnow;
	}
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(&tiocmget->waitq, &wait);

	return ret;
}