static inline void debug_hrtimer_deactivate(struct hrtimer *timer)
{
	debug_object_deactivate(timer, &hrtimer_debug_descr);
}