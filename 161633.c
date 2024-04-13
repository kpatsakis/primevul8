static void migrate_timer_list(struct timer_base *new_base, struct hlist_head *head)
{
	struct timer_list *timer;
	int cpu = new_base->cpu;

	while (!hlist_empty(head)) {
		timer = hlist_entry(head->first, struct timer_list, entry);
		detach_timer(timer, false);
		timer->flags = (timer->flags & ~TIMER_BASEMASK) | cpu;
		internal_add_timer(new_base, timer);
	}
}