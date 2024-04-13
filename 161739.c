static inline void hrtimer_update_next_timer(struct hrtimer_cpu_base *cpu_base,
					     struct hrtimer *timer)
{
#ifdef CONFIG_HIGH_RES_TIMERS
	cpu_base->next_timer = timer;
#endif
}