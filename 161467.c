static inline void debug_hrtimer_activate(struct hrtimer *timer)
{
	debug_object_activate(timer, &hrtimer_debug_descr);
}