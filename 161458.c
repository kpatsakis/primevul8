static inline int hrtimer_clockid_to_base(clockid_t clock_id)
{
	return hrtimer_clock_to_base_table[clock_id];
}