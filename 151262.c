void get_itv_value(struct record_header *record_hdr_curr,
		   struct record_header *record_hdr_prev,
		   unsigned long long *itv)
{
	/* Interval value in jiffies */
	*itv = get_interval(record_hdr_prev->uptime_cs,
			    record_hdr_curr->uptime_cs);
}