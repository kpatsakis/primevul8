static void perf_addr_filters_splice(struct perf_event *event,
				     struct list_head *head)
{
	unsigned long flags;
	LIST_HEAD(list);

	if (!has_addr_filter(event))
		return;

	/* don't bother with children, they don't have their own filters */
	if (event->parent)
		return;

	raw_spin_lock_irqsave(&event->addr_filters.lock, flags);

	list_splice_init(&event->addr_filters.list, &list);
	if (head)
		list_splice(head, &event->addr_filters.list);

	raw_spin_unlock_irqrestore(&event->addr_filters.lock, flags);

	free_filters_list(&list);
}