int datecmp(struct tm *rectime, struct tstamp *tse)
{
	if (rectime->tm_hour == tse->tm_hour) {
		if (rectime->tm_min == tse->tm_min)
			return (rectime->tm_sec - tse->tm_sec);
		else
			return (rectime->tm_min - tse->tm_min);
	}
	else
		return (rectime->tm_hour - tse->tm_hour);
}