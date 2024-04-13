static void __hrtimer_run_queues(struct hrtimer_cpu_base *cpu_base, ktime_t now)
{
	struct hrtimer_clock_base *base = cpu_base->clock_base;
	unsigned int active = cpu_base->active_bases;

	for (; active; base++, active >>= 1) {
		struct timerqueue_node *node;
		ktime_t basenow;

		if (!(active & 0x01))
			continue;

		basenow = ktime_add(now, base->offset);

		while ((node = timerqueue_getnext(&base->active))) {
			struct hrtimer *timer;

			timer = container_of(node, struct hrtimer, node);

			/*
			 * The immediate goal for using the softexpires is
			 * minimizing wakeups, not running timers at the
			 * earliest interrupt after their soft expiration.
			 * This allows us to avoid using a Priority Search
			 * Tree, which can answer a stabbing querry for
			 * overlapping intervals and instead use the simple
			 * BST we already have.
			 * We don't add extra wakeups by delaying timers that
			 * are right-of a not yet expired timer, because that
			 * timer will have to trigger a wakeup anyway.
			 */
			if (basenow < hrtimer_get_softexpires_tv64(timer))
				break;

			__run_hrtimer(cpu_base, base, timer, &basenow);
		}
	}
}