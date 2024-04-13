static void hrtimer_reprogram(struct hrtimer *timer,
			      struct hrtimer_clock_base *base)
{
	struct hrtimer_cpu_base *cpu_base = this_cpu_ptr(&hrtimer_bases);
	ktime_t expires = ktime_sub(hrtimer_get_expires(timer), base->offset);

	WARN_ON_ONCE(hrtimer_get_expires_tv64(timer) < 0);

	/*
	 * If the timer is not on the current cpu, we cannot reprogram
	 * the other cpus clock event device.
	 */
	if (base->cpu_base != cpu_base)
		return;

	/*
	 * If the hrtimer interrupt is running, then it will
	 * reevaluate the clock bases and reprogram the clock event
	 * device. The callbacks are always executed in hard interrupt
	 * context so we don't need an extra check for a running
	 * callback.
	 */
	if (cpu_base->in_hrtirq)
		return;

	/*
	 * CLOCK_REALTIME timer might be requested with an absolute
	 * expiry time which is less than base->offset. Set it to 0.
	 */
	if (expires < 0)
		expires = 0;

	if (expires >= cpu_base->expires_next)
		return;

	/* Update the pointer to the next expiring timer */
	cpu_base->next_timer = timer;

	/*
	 * If a hang was detected in the last timer interrupt then we
	 * do not schedule a timer which is earlier than the expiry
	 * which we enforced in the hang detection. We want the system
	 * to make progress.
	 */
	if (cpu_base->hang_detected)
		return;

	/*
	 * Program the timer hardware. We enforce the expiry for
	 * events which are already in the past.
	 */
	cpu_base->expires_next = expires;
	tick_program_event(expires, 1);
}