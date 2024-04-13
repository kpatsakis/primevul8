void usb_sg_wait(struct usb_sg_request *io)
{
	int i;
	int entries = io->entries;

	/* queue the urbs.  */
	spin_lock_irq(&io->lock);
	i = 0;
	while (i < entries && !io->status) {
		int retval;

		io->urbs[i]->dev = io->dev;
		spin_unlock_irq(&io->lock);

		retval = usb_submit_urb(io->urbs[i], GFP_NOIO);

		switch (retval) {
			/* maybe we retrying will recover */
		case -ENXIO:	/* hc didn't queue this one */
		case -EAGAIN:
		case -ENOMEM:
			retval = 0;
			yield();
			break;

			/* no error? continue immediately.
			 *
			 * NOTE: to work better with UHCI (4K I/O buffer may
			 * need 3K of TDs) it may be good to limit how many
			 * URBs are queued at once; N milliseconds?
			 */
		case 0:
			++i;
			cpu_relax();
			break;

			/* fail any uncompleted urbs */
		default:
			io->urbs[i]->status = retval;
			dev_dbg(&io->dev->dev, "%s, submit --> %d\n",
				__func__, retval);
			usb_sg_cancel(io);
		}
		spin_lock_irq(&io->lock);
		if (retval && (io->status == 0 || io->status == -ECONNRESET))
			io->status = retval;
	}
	io->count -= entries - i;
	if (io->count == 0)
		complete(&io->complete);
	spin_unlock_irq(&io->lock);

	/* OK, yes, this could be packaged as non-blocking.
	 * So could the submit loop above ... but it's easier to
	 * solve neither problem than to solve both!
	 */
	wait_for_completion(&io->complete);

	sg_clean(io);
}