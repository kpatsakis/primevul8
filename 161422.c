static inline void debug_hrtimer_init(struct hrtimer *timer)
{
	debug_object_init(timer, &hrtimer_debug_descr);
}