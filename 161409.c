get_target_base(struct timer_base *base, unsigned tflags)
{
	return get_timer_this_cpu_base(tflags);
}