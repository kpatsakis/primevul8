hrtimer_force_reprogram(struct hrtimer_cpu_base *cpu_base, int skip_equal)
{
	ktime_t expires_next;

	if (!cpu_base->hres_active)
		return;

	expires_next = __hrtimer_get_next_event(cpu_base);

	if (skip_equal && expires_next == cpu_base->expires_next)
		return;

	cpu_base->expires_next = expires_next;

	/*
	 * If a hang was detected in the last timer interrupt then we
	 * leave the hang delay active in the hardware. We want the
	 * system to make progress. That also prevents the following
	 * scenario:
	 * T1 expires 50ms from now
	 * T2 expires 5s from now
	 *
	 * T1 is removed, so this code is called and would reprogram
	 * the hardware to 5s from now. Any hrtimer_start after that
	 * will not reprogram the hardware due to hang_detected being
	 * set. So we'd effectivly block all timers until the T2 event
	 * fires.
	 */
	if (cpu_base->hang_detected)
		return;

	tick_program_event(cpu_base->expires_next, 1);
}