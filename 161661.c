debug_init(struct hrtimer *timer, clockid_t clockid,
	   enum hrtimer_mode mode)
{
	debug_hrtimer_init(timer);
	trace_hrtimer_init(timer, clockid, mode);
}