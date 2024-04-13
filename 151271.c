int decode_timestamp(char timestamp[], struct tstamp *tse)
{
	timestamp[2] = timestamp[5] = '\0';
	tse->tm_sec  = atoi(&timestamp[6]);
	tse->tm_min  = atoi(&timestamp[3]);
	tse->tm_hour = atoi(timestamp);

	if ((tse->tm_sec < 0) || (tse->tm_sec > 59) ||
	    (tse->tm_min < 0) || (tse->tm_min > 59) ||
	    (tse->tm_hour < 0) || (tse->tm_hour > 23))
		return 1;

	tse->use = TRUE;

	return 0;
}