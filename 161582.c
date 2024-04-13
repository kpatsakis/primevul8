print_active_timers(struct seq_file *m, struct hrtimer_clock_base *base,
		    u64 now)
{
	struct hrtimer *timer, tmp;
	unsigned long next = 0, i;
	struct timerqueue_node *curr;
	unsigned long flags;

next_one:
	i = 0;
	raw_spin_lock_irqsave(&base->cpu_base->lock, flags);

	curr = timerqueue_getnext(&base->active);
	/*
	 * Crude but we have to do this O(N*N) thing, because
	 * we have to unlock the base when printing:
	 */
	while (curr && i < next) {
		curr = timerqueue_iterate_next(curr);
		i++;
	}

	if (curr) {

		timer = container_of(curr, struct hrtimer, node);
		tmp = *timer;
		raw_spin_unlock_irqrestore(&base->cpu_base->lock, flags);

		print_timer(m, timer, &tmp, i, now);
		next++;
		goto next_one;
	}
	raw_spin_unlock_irqrestore(&base->cpu_base->lock, flags);
}