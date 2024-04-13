static inline int __hrtimer_hres_active(struct hrtimer_cpu_base *cpu_base)
{
	return cpu_base->hres_active;
}