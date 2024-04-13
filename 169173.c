int write_stats(int curr, int read_from_file, long *cnt, int use_tm_start,
		int use_tm_end, int reset, unsigned int act_id, int reset_cd)
{
	int i, prev_hour;
	unsigned long long itv;
	static int cross_day = 0;

	if (reset_cd) {
		/*
		 * cross_day is a static variable that is set to 1 when the first
		 * record of stats from a new day is read from a unique data file
		 * (in the case where the file contains data from two consecutive
		 * days). When set to 1, every following records timestamp will
		 * have its hour value increased by 24.
		 * Yet when a new activity (being read from the file) is going to
		 * be displayed, we start reading the file from the beginning
		 * again, and so cross_day should be reset in this case.
		 */
		cross_day = 0;
	}

	/* Check time (1) */
	if (read_from_file) {
		if (!next_slice(record_hdr[2].uptime_cs, record_hdr[curr].uptime_cs,
				reset, interval))
			/* Not close enough to desired interval */
			return 0;
	}

	/* Get then set previous timestamp */
	if (sa_get_record_timestamp_struct(flags + S_F_LOCAL_TIME, &record_hdr[!curr],
					   &rectime))
		return 0;
	prev_hour = rectime.tm_hour;
	set_record_timestamp_string(flags, &record_hdr[!curr],
				    NULL, timestamp[!curr], TIMESTAMP_LEN, &rectime);

	/* Get then set current timestamp */
	if (sa_get_record_timestamp_struct(flags + S_F_LOCAL_TIME, &record_hdr[curr],
					   &rectime))
		return 0;
	set_record_timestamp_string(flags, &record_hdr[curr],
				    NULL, timestamp[curr], TIMESTAMP_LEN, &rectime);

	/*
	 * Check if we are beginning a new day.
	 * Use rectime.tm_hour and prev_hour instead of record_hdr[].hour for comparison
	 * to take into account the current timezone (hours displayed will depend on the
	 * TZ variable value).
	 */
	if (use_tm_start && record_hdr[!curr].ust_time &&
	    (record_hdr[curr].ust_time > record_hdr[!curr].ust_time) &&
	    (rectime.tm_hour < prev_hour)) {
		cross_day = 1;
	}

	/* Check time (2) */
	if (use_tm_start && (datecmp(&rectime, &tm_start, cross_day) < 0))
		/* it's too soon... */
		return 0;

	/* Get interval value in 1/100th of a second */
	get_itv_value(&record_hdr[curr], &record_hdr[!curr], &itv);

	/* Check time (3) */
	if (use_tm_end && (datecmp(&rectime, &tm_end, cross_day) > 0)) {
		/* It's too late... */
		*cnt = 0;
		return 0;
	}

	avg_count++;

	/* Test stdout */
	TEST_STDOUT(STDOUT_FILENO);

	for (i = 0; i < NR_ACT; i++) {

		if ((act_id != ALL_ACTIVITIES) && (act[i]->id != act_id))
			continue;

		if (IS_SELECTED(act[i]->options) && (act[i]->nr[curr] > 0)) {
			/* Display current activity statistics */
			(*act[i]->f_print)(act[i], !curr, curr, itv);
		}
	}

	return 1;
}