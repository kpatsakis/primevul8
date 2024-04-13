static void snd_timer_reschedule(struct snd_timer * timer, unsigned long ticks_left)
{
	struct snd_timer_instance *ti;
	unsigned long ticks = ~0UL;

	list_for_each_entry(ti, &timer->active_list_head, active_list) {
		if (ti->flags & SNDRV_TIMER_IFLG_START) {
			ti->flags &= ~SNDRV_TIMER_IFLG_START;
			ti->flags |= SNDRV_TIMER_IFLG_RUNNING;
			timer->running++;
		}
		if (ti->flags & SNDRV_TIMER_IFLG_RUNNING) {
			if (ticks > ti->cticks)
				ticks = ti->cticks;
		}
	}
	if (ticks == ~0UL) {
		timer->flags &= ~SNDRV_TIMER_FLG_RESCHED;
		return;
	}
	if (ticks > timer->hw.ticks)
		ticks = timer->hw.ticks;
	if (ticks_left != ticks)
		timer->flags |= SNDRV_TIMER_FLG_CHANGE;
	timer->sticks = ticks;
}