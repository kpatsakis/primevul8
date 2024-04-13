int check_disk_reg(struct activity *a, int curr, int ref, int pos)
{
	struct stats_disk *sdc, *sdp;
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

	sdc = (struct stats_disk *) ((char *) a->buf[curr] + pos * a->msize);

	do {
		sdp = (struct stats_disk *) ((char *) a->buf[ref] + j * a->msize);

		if ((sdc->major == sdp->major) &&
		    (sdc->minor == sdp->minor)) {
			/*
			 * Disk found.
			 * If all the counters have decreased then the likelyhood
			 * is that the disk has been unregistered and a new disk inserted.
			 * If only one or two have decreased then the likelyhood
			 * is that the counter has simply wrapped.
			 */
			if ((sdc->nr_ios < sdp->nr_ios) &&
			    (sdc->rd_sect < sdp->rd_sect) &&
			    (sdc->wr_sect < sdp->wr_sect))
				/* Same device registered again */
				return -2;

			return j;
		}
		if (++j >= a->nr[ref]) {
			j = 0;
		}
	}
	while (j != j0);

	/* This is a newly registered device */
	return -1;
}