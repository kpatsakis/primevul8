time_t get_time_ref(void)
{
	struct tm ltm;
	time_t t;

	if (DISPLAY_ONE_DAY(flags)) {
		localtime_r((time_t *) &(record_hdr[2].ust_time), &ltm);

		/* Move back to midnight */
		ltm.tm_sec = ltm.tm_min = ltm.tm_hour = 0;

		t = mktime(&ltm);
		if (t != -1)
			return t;
	}

	return (time_t) record_hdr[2].ust_time;
}