static int enqueue_hrtimer(struct hrtimer *timer,
			   struct hrtimer_clock_base *base)
{
	debug_activate(timer);

	base->cpu_base->active_bases |= 1 << base->index;

	timer->state = HRTIMER_STATE_ENQUEUED;

	return timerqueue_add(&base->active, &timer->node);
}