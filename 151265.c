double compute_ifutil(struct stats_net_dev *st_net_dev, double rx, double tx)
{
	unsigned long long speed;

	if (st_net_dev->speed) {

		speed = (unsigned long long) st_net_dev->speed * 1000000;

		if (st_net_dev->duplex == C_DUPLEX_FULL) {
			/* Full duplex */
			if (rx > tx) {
				return (rx * 800 / speed);
			}
			else {
				return (tx * 800 / speed);
			}
		}
		else {
			/* Half duplex */
			return ((rx + tx) * 800 / speed);
		}
	}

	return 0;
}