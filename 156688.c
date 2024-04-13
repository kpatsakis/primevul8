static int32_t get_time_diff(struct timeval *tv)
{
	struct timeval now;
	int32_t hsec;

	gettimeofday(&now, NULL);

	hsec = (now.tv_sec - tv->tv_sec) * 100;
	hsec += (now.tv_usec - tv->tv_usec) / 10000;

	return hsec;
}