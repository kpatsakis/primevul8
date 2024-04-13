static void update_avg(u64 *avg, u64 sample)
{
	s64 diff = sample - *avg;
	*avg += diff >> 3;
}