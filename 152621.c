void write_stats_avg(int curr, int read_from_file, unsigned int act_id)
{
	int i;
	unsigned long long itv;

	/* Interval value in 1/100th of a second */
	itv = get_interval(record_hdr[2].uptime_cs, record_hdr[curr].uptime_cs);

	strncpy(timestamp[curr], _("Average:"), TIMESTAMP_LEN);
	timestamp[curr][TIMESTAMP_LEN - 1] = '\0';
	memcpy(timestamp[!curr], timestamp[curr], TIMESTAMP_LEN);

	/* Test stdout */
	TEST_STDOUT(STDOUT_FILENO);

	for (i = 0; i < NR_ACT; i++) {

		if ((act_id != ALL_ACTIVITIES) && (act[i]->id != act_id))
			continue;

		if (IS_SELECTED(act[i]->options) && (act[i]->nr[curr] > 0)) {
			/* Display current average activity statistics */
			(*act[i]->f_print_avg)(act[i], 2, curr, itv);
		}
	}

	if (read_from_file) {
		/*
		 * Reset number of lines printed only if we read stats
		 * from a system activity file.
		 */
		avg_count = 0;
	}
}