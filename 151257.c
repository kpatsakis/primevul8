int check_net_dev_reg(struct activity *a, int curr, int ref, int pos)
{
	struct stats_net_dev *sndc, *sndp;
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

	sndc = (struct stats_net_dev *) ((char *) a->buf[curr] + pos * a->msize);

	do {
		sndp = (struct stats_net_dev *) ((char *) a->buf[ref] + j * a->msize);

		if (!strcmp(sndc->interface, sndp->interface)) {
			/*
			 * Network interface found.
			 * If a counter has decreased, then we may assume that the
			 * corresponding interface was unregistered, then registered again.
			 */
			if ((sndc->rx_packets    < sndp->rx_packets)    ||
			    (sndc->tx_packets    < sndp->tx_packets)    ||
			    (sndc->rx_bytes      < sndp->rx_bytes)      ||
			    (sndc->tx_bytes      < sndp->tx_bytes)      ||
			    (sndc->rx_compressed < sndp->rx_compressed) ||
			    (sndc->tx_compressed < sndp->tx_compressed) ||
			    (sndc->multicast     < sndp->multicast)) {

				/*
				 * Special processing for rx_bytes (_packets) and
				 * tx_bytes (_packets) counters: If the number of
				 * bytes (packets) has decreased, whereas the number of
				 * packets (bytes) has increased, then assume that the
				 * relevant counter has met an overflow condition, and that
				 * the interface was not unregistered, which is all the
				 * more plausible that the previous value for the counter
				 * was > ULLONG_MAX/2.
				 * NB: the average value displayed will be wrong in this case...
				 *
				 * If such an overflow is detected, just set the flag. There is no
				 * need to handle this in a special way: the difference is still
				 * properly calculated if the result is of the same type (i.e.
				 * unsigned long) as the two values.
				 */
				int ovfw = FALSE;

				if ((sndc->rx_bytes   < sndp->rx_bytes)   &&
				    (sndc->rx_packets > sndp->rx_packets) &&
				    (sndp->rx_bytes   > (~0ULL >> 1))) {
					ovfw = TRUE;
				}
				if ((sndc->tx_bytes   < sndp->tx_bytes)   &&
				    (sndc->tx_packets > sndp->tx_packets) &&
				    (sndp->tx_bytes   > (~0ULL >> 1))) {
					ovfw = TRUE;
				}
				if ((sndc->rx_packets < sndp->rx_packets) &&
				    (sndc->rx_bytes   > sndp->rx_bytes)   &&
				    (sndp->rx_packets > (~0ULL >> 1))) {
					ovfw = TRUE;
				}
				if ((sndc->tx_packets < sndp->tx_packets) &&
				    (sndc->tx_bytes   > sndp->tx_bytes)   &&
				    (sndp->tx_packets > (~0ULL >> 1))) {
					ovfw = TRUE;
				}

				if (!ovfw)
					/*
					 * OK: Assume here that the device was
					 * actually unregistered.
					 */
					return -2;
			}
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