void destroy_hrtimer_on_stack(struct hrtimer *timer)
{
	debug_object_free(timer, &hrtimer_debug_descr);
}