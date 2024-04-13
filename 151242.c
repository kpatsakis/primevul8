int datecmp(struct tm *rectime, struct tstamp *tse, int cross_day)
{
	int tm_hour = rectime->tm_hour;

	if (cross_day) {
		/*
		 * This is necessary if we want to properly handle something like:
		 * sar -s time_start -e time_end with
		 * time_start(day D) > time_end(day D+1)
		 */
		tm_hour += 24;
	}

	if (tm_hour == tse->tm_hour) {
		if (rectime->tm_min == tse->tm_min)
			return (rectime->tm_sec - tse->tm_sec);
		else
			return (rectime->tm_min - tse->tm_min);
	}
	else
		return (tm_hour - tse->tm_hour);
}