static void retrigger_next_event(void *arg)
{
	struct hrtimer_cpu_base *base = this_cpu_ptr(&hrtimer_bases);

	if (!base->hres_active)
		return;

	raw_spin_lock(&base->lock);
	hrtimer_update_base(base);
	hrtimer_force_reprogram(base, 0);
	raw_spin_unlock(&base->lock);
}