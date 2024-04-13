void write_stats_startup(int curr)
{
	int i;

	/* Set to 0 previous structures corresponding to boot time */
	memset(&record_hdr[!curr], 0, RECORD_HEADER_SIZE);
	record_hdr[!curr].record_type = R_STATS;
	record_hdr[!curr].hour        = record_hdr[curr].hour;
	record_hdr[!curr].minute      = record_hdr[curr].minute;
	record_hdr[!curr].second      = record_hdr[curr].second;
	record_hdr[!curr].ust_time    = record_hdr[curr].ust_time;

	for (i = 0; i < NR_ACT; i++) {
		if (IS_SELECTED(act[i]->options) && (act[i]->nr[curr] > 0)) {
			/*
			 * Using nr[curr] and not nr[!curr] below because we initialize
			 * reference structures for each structure that has been
			 * currently read in memory.
			 * No problem with buffers allocation since they all have the
			 * same size.
			 */
			memset(act[i]->buf[!curr], 0,
			       (size_t) act[i]->msize * (size_t) act[i]->nr[curr] * (size_t) act[i]->nr2);
		}
	}

	flags |= S_F_SINCE_BOOT;
	dis = TRUE;

	write_stats(curr, USE_SADC, &count, NO_TM_START, NO_TM_END, NO_RESET,
		    ALL_ACTIVITIES, TRUE);

	exit(0);
}