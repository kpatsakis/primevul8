static inline void hrtimer_init_hres(struct hrtimer_cpu_base *base)
{
	base->expires_next = KTIME_MAX;
	base->hres_active = 0;
}