int sa_get_record_timestamp_struct(unsigned int l_flags, struct record_header *record_hdr,
				   struct tm *rectime, struct tm *loctime)
{
	struct tm *ltm = NULL;
	int rc = 0;

	/* Fill localtime structure if given */
	if (loctime) {
		ltm = localtime_r((const time_t *) &(record_hdr->ust_time), loctime);
		if (ltm) {
			/* Done so that we have some default values */
			*rectime = *loctime;
		}
		else {
			rc = 1;
		}
	}

	/* Fill generic rectime structure */
	if (PRINT_LOCAL_TIME(l_flags) && !ltm) {
		/* Get local time if not already done */
		ltm = localtime_r((const time_t *) &(record_hdr->ust_time), rectime);
	}

	if (!PRINT_LOCAL_TIME(l_flags) && !PRINT_TRUE_TIME(l_flags)) {
		/*
		 * Get time in UTC
		 * (the user doesn't want local time nor time of file's creator).
		 */
		ltm = gmtime_r((const time_t *) &(record_hdr->ust_time), rectime);
	}

	if (!ltm) {
		rc = 1;
	}

	if (PRINT_TRUE_TIME(l_flags)) {
		/* Time of file's creator */
		rectime->tm_hour = record_hdr->hour;
		rectime->tm_min  = record_hdr->minute;
		rectime->tm_sec  = record_hdr->second;
	}

	return rc;
}