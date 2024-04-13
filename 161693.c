static ktime_t __hrtimer_get_next_event(struct hrtimer_cpu_base *cpu_base)
{
	struct hrtimer_clock_base *base = cpu_base->clock_base;
	unsigned int active = cpu_base->active_bases;
	ktime_t expires, expires_next = KTIME_MAX;

	hrtimer_update_next_timer(cpu_base, NULL);
	for (; active; base++, active >>= 1) {
		struct timerqueue_node *next;
		struct hrtimer *timer;

		if (!(active & 0x01))
			continue;

		next = timerqueue_getnext(&base->active);
		timer = container_of(next, struct hrtimer, node);
		expires = ktime_sub(hrtimer_get_expires(timer), base->offset);
		if (expires < expires_next) {
			expires_next = expires;
			hrtimer_update_next_timer(cpu_base, timer);
		}
	}
	/*
	 * clock_was_set() might have changed base->offset of any of
	 * the clock bases so the result might be negative. Fix it up
	 * to prevent a false positive in clockevents_program_event().
	 */
	if (expires_next < 0)
		expires_next = 0;
	return expires_next;
}