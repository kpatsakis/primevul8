int check_net_edev_reg(struct activity *a, int curr, int ref, int pos)
{
	struct stats_net_edev *snedc, *snedp;
	int j0, j = pos;

	if (!a->nr[ref])
		/*
		 * No items found in previous iteration:
		 * Current interface is necessarily new.
		 */
		return -1;

	if (j >= a->nr[ref]) {
		j = a->nr[ref] - 1;
	}
	j0 = j;

	snedc = (struct stats_net_edev *) ((char *) a->buf[curr] + pos * a->msize);

	do {
		snedp = (struct stats_net_edev *) ((char *) a->buf[ref] + j * a->msize);

		if (!strcmp(snedc->interface, snedp->interface)) {
			/*
			 * Network interface found.
			 * If a counter has decreased, then we may assume that the
			 * corresponding interface was unregistered, then registered again.
			 */
			if ((snedc->tx_errors         < snedp->tx_errors)         ||
			    (snedc->collisions        < snedp->collisions)        ||
			    (snedc->rx_dropped        < snedp->rx_dropped)        ||
			    (snedc->tx_dropped        < snedp->tx_dropped)        ||
			    (snedc->tx_carrier_errors < snedp->tx_carrier_errors) ||
			    (snedc->rx_frame_errors   < snedp->rx_frame_errors)   ||
			    (snedc->rx_fifo_errors    < snedp->rx_fifo_errors)    ||
			    (snedc->tx_fifo_errors    < snedp->tx_fifo_errors))
				/*
				 * OK: assume here that the device was
				 * actually unregistered.
				 */
				return -2;

			return j;
		}
		if (++j >= a->nr[ref]) {
			j = 0;
		}
	}
	while (j != j0);

	/* This is a newly registered interface */
	return -1;
}