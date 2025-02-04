remove_hrtimer(struct hrtimer *timer, struct hrtimer_clock_base *base, bool restart)
{
	if (hrtimer_is_queued(timer)) {
		u8 state = timer->state;
		int reprogram;

		/*
		 * Remove the timer and force reprogramming when high
		 * resolution mode is active and the timer is on the current
		 * CPU. If we remove a timer on another CPU, reprogramming is
		 * skipped. The interrupt event on this CPU is fired and
		 * reprogramming happens in the interrupt handler. This is a
		 * rare case and less expensive than a smp call.
		 */
		debug_deactivate(timer);
		reprogram = base->cpu_base == this_cpu_ptr(&hrtimer_bases);

		if (!restart)
			state = HRTIMER_STATE_INACTIVE;

		__remove_hrtimer(timer, base, state, reprogram);
		return 1;
	}
	return 0;
}