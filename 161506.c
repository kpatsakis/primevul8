static inline void debug_hrtimer_free(struct hrtimer *timer)
{
	debug_object_free(timer, &hrtimer_debug_descr);
}