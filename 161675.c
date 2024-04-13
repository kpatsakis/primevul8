u64 hrtimer_get_next_event(void)
{
	struct hrtimer_cpu_base *cpu_base = this_cpu_ptr(&hrtimer_bases);
	u64 expires = KTIME_MAX;
	unsigned long flags;

	raw_spin_lock_irqsave(&cpu_base->lock, flags);

	if (!__hrtimer_hres_active(cpu_base))
		expires = __hrtimer_get_next_event(cpu_base);

	raw_spin_unlock_irqrestore(&cpu_base->lock, flags);

	return expires;
}