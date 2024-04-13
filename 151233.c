int next_slice(unsigned long long uptime_ref, unsigned long long uptime,
	       int reset, long interval)
{
	unsigned long file_interval, entry;
	static unsigned long long last_uptime = 0;
	int min, max, pt1, pt2;
	double f;

	/* uptime is expressed in 1/100th of a second */
	if (!last_uptime || reset) {
		last_uptime = uptime_ref;
	}

	/* Interval cannot be greater than 0xffffffff here */
	f = ((double) ((uptime - last_uptime) & 0xffffffff)) / 100;
	file_interval = (unsigned long) f;
	if ((f * 10) - (file_interval * 10) >= 5) {
		file_interval++; /* Rounding to correct value */
	}

	last_uptime = uptime;

	/*
	 * A few notes about the "algorithm" used here to display selected entries
	 * from the system activity file (option -f with -i flag):
	 * Let 'Iu' be the interval value given by the user on the command line,
	 *     'If' the interval between current and previous line in the system
	 * activity file,
	 * and 'En' the nth entry (identified by its time stamp) of the file.
	 * We choose In = [ En - If/2, En + If/2 [ if If is even,
	 *        or In = [ En - If/2, En + If/2 ] if not.
	 * En will be displayed if
	 *       (Pn * Iu) or (P'n * Iu) belongs to In
	 * with  Pn = En / Iu and P'n = En / Iu + 1
	 */
	f = ((double) ((uptime - uptime_ref) & 0xffffffff)) / 100;
	entry = (unsigned long) f;
	if ((f * 10) - (entry * 10) >= 5) {
		entry++;
	}

	min = entry - (file_interval / 2);
	max = entry + (file_interval / 2) + (file_interval & 0x1);
	pt1 = (entry / interval) * interval;
	pt2 = ((entry / interval) + 1) * interval;

	return (((pt1 >= min) && (pt1 < max)) || ((pt2 >= min) && (pt2 < max)));
}