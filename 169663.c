static void detach_sb_event(struct perf_event *event)
{
	struct pmu_event_list *pel = per_cpu_ptr(&pmu_sb_events, event->cpu);

	raw_spin_lock(&pel->lock);
	list_del_rcu(&event->sb_list);
	raw_spin_unlock(&pel->lock);
}