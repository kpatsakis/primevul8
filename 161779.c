void hrtimer_run_queues(void)
{
	struct hrtimer_cpu_base *cpu_base = this_cpu_ptr(&hrtimer_bases);
	ktime_t now;

	if (__hrtimer_hres_active(cpu_base))
		return;

	/*
	 * This _is_ ugly: We have to check periodically, whether we
	 * can switch to highres and / or nohz mode. The clocksource
	 * switch happens with xtime_lock held. Notification from
	 * there only sets the check bit in the tick_oneshot code,
	 * otherwise we might deadlock vs. xtime_lock.
	 */
	if (tick_check_oneshot_change(!hrtimer_is_hres_enabled())) {
		hrtimer_switch_to_hres();
		return;
	}

	raw_spin_lock(&cpu_base->lock);
	now = hrtimer_update_base(cpu_base);
	__hrtimer_run_queues(cpu_base, now);
	raw_spin_unlock(&cpu_base->lock);
}