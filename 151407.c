clock_filter(
	struct peer *peer,		/* peer structure pointer */
	double	sample_offset,		/* clock offset */
	double	sample_delay,		/* roundtrip delay */
	double	sample_disp		/* dispersion */
	)
{
	double	dst[NTP_SHIFT];		/* distance vector */
	int	ord[NTP_SHIFT];		/* index vector */
	int	i, j, k, m;
	double	dtemp, etemp;
	char	tbuf[80];

	/*
	 * A sample consists of the offset, delay, dispersion and epoch
	 * of arrival. The offset and delay are determined by the on-
	 * wire protocol. The dispersion grows from the last outbound
	 * packet to the arrival of this one increased by the sum of the
	 * peer precision and the system precision as required by the
	 * error budget. First, shift the new arrival into the shift
	 * register discarding the oldest one.
	 */
	j = peer->filter_nextpt;
	peer->filter_offset[j] = sample_offset;
	peer->filter_delay[j] = sample_delay;
	peer->filter_disp[j] = sample_disp;
	peer->filter_epoch[j] = current_time;
	j = (j + 1) % NTP_SHIFT;
	peer->filter_nextpt = j;

	/*
	 * Update dispersions since the last update and at the same
	 * time initialize the distance and index lists. Since samples
	 * become increasingly uncorrelated beyond the Allan intercept,
	 * only under exceptional cases will an older sample be used.
	 * Therefore, the distance list uses a compound metric. If the
	 * dispersion is greater than the maximum dispersion, clamp the
	 * distance at that value. If the time since the last update is
	 * less than the Allan intercept use the delay; otherwise, use
	 * the sum of the delay and dispersion.
	 */
	dtemp = clock_phi * (current_time - peer->update);
	peer->update = current_time;
	for (i = NTP_SHIFT - 1; i >= 0; i--) {
		if (i != 0)
			peer->filter_disp[j] += dtemp;
		if (peer->filter_disp[j] >= MAXDISPERSE) {
			peer->filter_disp[j] = MAXDISPERSE;
			dst[i] = MAXDISPERSE;
		} else if (peer->update - peer->filter_epoch[j] >
		    (u_long)ULOGTOD(allan_xpt)) {
			dst[i] = peer->filter_delay[j] +
			    peer->filter_disp[j];
		} else {
			dst[i] = peer->filter_delay[j];
		}
		ord[i] = j;
		j = (j + 1) % NTP_SHIFT;
	}

	/*
	 * If the clock has stabilized, sort the samples by distance.
	 */
	if (freq_cnt == 0) {
		for (i = 1; i < NTP_SHIFT; i++) {
			for (j = 0; j < i; j++) {
				if (dst[j] > dst[i]) {
					k = ord[j];
					ord[j] = ord[i];
					ord[i] = k;
					etemp = dst[j];
					dst[j] = dst[i];
					dst[i] = etemp;
				}
			}
		}
	}

	/*
	 * Copy the index list to the association structure so ntpq
	 * can see it later. Prune the distance list to leave only
	 * samples less than the maximum dispersion, which disfavors
	 * uncorrelated samples older than the Allan intercept. To
	 * further improve the jitter estimate, of the remainder leave
	 * only samples less than the maximum distance, but keep at
	 * least two samples for jitter calculation.
	 */
	m = 0;
	for (i = 0; i < NTP_SHIFT; i++) {
		peer->filter_order[i] = (u_char) ord[i];
		if (dst[i] >= MAXDISPERSE || (m >= 2 && dst[i] >=
		    sys_maxdist))
			continue;
		m++;
	}

	/*
	 * Compute the dispersion and jitter. The dispersion is weighted
	 * exponentially by NTP_FWEIGHT (0.5) so it is normalized close
	 * to 1.0. The jitter is the RMS differences relative to the
	 * lowest delay sample.
	 */
	peer->disp = peer->jitter = 0;
	k = ord[0];
	for (i = NTP_SHIFT - 1; i >= 0; i--) {
		j = ord[i];
		peer->disp = NTP_FWEIGHT * (peer->disp +
		    peer->filter_disp[j]);
		if (i < m)
			peer->jitter += DIFF(peer->filter_offset[j],
			    peer->filter_offset[k]);
	}

	/*
	 * If no acceptable samples remain in the shift register,
	 * quietly tiptoe home leaving only the dispersion. Otherwise,
	 * save the offset, delay and jitter. Note the jitter must not
	 * be less than the precision.
	 */
	if (m == 0) {
		clock_select();
		return;
	}
	etemp = fabs(peer->offset - peer->filter_offset[k]);
	peer->offset = peer->filter_offset[k];
	peer->delay = peer->filter_delay[k];
	if (m > 1)
		peer->jitter /= m - 1;
	peer->jitter = max(SQRT(peer->jitter), LOGTOD(sys_precision));

	/*
	 * If the the new sample and the current sample are both valid
	 * and the difference between their offsets exceeds CLOCK_SGATE
	 * (3) times the jitter and the interval between them is less
	 * than twice the host poll interval, consider the new sample
	 * a popcorn spike and ignore it.
	 */
	if (peer->disp < sys_maxdist && peer->filter_disp[k] <
	    sys_maxdist && etemp > CLOCK_SGATE * peer->jitter &&
	    peer->filter_epoch[k] - peer->epoch < 2. *
	    ULOGTOD(peer->hpoll)) {
		snprintf(tbuf, sizeof(tbuf), "%.6f s", etemp);
		report_event(PEVNT_POPCORN, peer, tbuf);
		return;
	}

	/*
	 * A new minimum sample is useful only if it is later than the
	 * last one used. In this design the maximum lifetime of any
	 * sample is not greater than eight times the poll interval, so
	 * the maximum interval between minimum samples is eight
	 * packets.
	 */
	if (peer->filter_epoch[k] <= peer->epoch) {
#if DEBUG
	if (debug > 1)
		printf("clock_filter: old sample %lu\n", current_time -
		    peer->filter_epoch[k]);
#endif
		return;
	}
	peer->epoch = peer->filter_epoch[k];

	/*
	 * The mitigated sample statistics are saved for later
	 * processing. If not synchronized or not in a burst, tickle the
	 * clock select algorithm.
	 */
	record_peer_stats(&peer->srcadr, ctlpeerstatus(peer),
	    peer->offset, peer->delay, peer->disp, peer->jitter);
#ifdef DEBUG
	if (debug)
		printf(
		    "clock_filter: n %d off %.6f del %.6f dsp %.6f jit %.6f\n",
		    m, peer->offset, peer->delay, peer->disp,
		    peer->jitter);
#endif
	if (peer->burst == 0 || sys_leap == LEAP_NOTINSYNC)
		clock_select();
}