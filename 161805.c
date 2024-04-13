static void __remove_hrtimer(struct hrtimer *timer,
			     struct hrtimer_clock_base *base,
			     u8 newstate, int reprogram)
{
	struct hrtimer_cpu_base *cpu_base = base->cpu_base;
	u8 state = timer->state;

	timer->state = newstate;
	if (!(state & HRTIMER_STATE_ENQUEUED))
		return;

	if (!timerqueue_del(&base->active, &timer->node))
		cpu_base->active_bases &= ~(1 << base->index);

#ifdef CONFIG_HIGH_RES_TIMERS
	/*
	 * Note: If reprogram is false we do not update
	 * cpu_base->next_timer. This happens when we remove the first
	 * timer on a remote cpu. No harm as we never dereference
	 * cpu_base->next_timer. So the worst thing what can happen is
	 * an superflous call to hrtimer_force_reprogram() on the
	 * remote cpu later on if the same timer gets enqueued again.
	 */
	if (reprogram && timer == cpu_base->next_timer)
		hrtimer_force_reprogram(cpu_base, 1);
#endif
}