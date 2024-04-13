static inline int hrtimer_hres_active(void)
{
	return __hrtimer_hres_active(this_cpu_ptr(&hrtimer_bases));
}