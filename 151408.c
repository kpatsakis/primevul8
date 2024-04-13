root_distance(
	struct peer *peer	/* peer structure pointer */
	)
{
	double	dtemp;

	/*
	 * Root Distance (LAMBDA) is defined as:
	 * (delta + DELTA)/2 + epsilon + EPSILON + phi
	 *
	 * where:
	 *  delta   is the round-trip delay
	 *  DELTA   is the root delay
	 *  epsilon is the remote server precision + local precision
	 *	    + (15 usec each second)
	 *  EPSILON is the root dispersion
	 *  phi     is the peer jitter statistic
	 *
	 * NB: Think hard about why we are using these values, and what
	 * the alternatives are, and the various pros/cons.
	 *
	 * DLM thinks these are probably the best choices from any of the
	 * other worse choices.
	 */
	dtemp = (peer->delay + peer->rootdelay) / 2
		+ LOGTOD(peer->precision)
		  + LOGTOD(sys_precision)
		  + clock_phi * (current_time - peer->update)
		+ peer->rootdisp
		+ peer->jitter;
	/*
	 * Careful squeak here. The value returned must be greater than
	 * the minimum root dispersion in order to avoid clockhop with
	 * highly precise reference clocks. Note that the root distance
	 * cannot exceed the sys_maxdist, as this is the cutoff by the
	 * selection algorithm.
	 */
	if (dtemp < sys_mindisp)
		dtemp = sys_mindisp;
	return (dtemp);
}