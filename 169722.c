static void perf_iterate_sb_cpu(perf_iterate_f output, void *data)
{
	struct pmu_event_list *pel = this_cpu_ptr(&pmu_sb_events);
	struct perf_event *event;

	list_for_each_entry_rcu(event, &pel->list, sb_list) {
		/*
		 * Skip events that are not fully formed yet; ensure that
		 * if we observe event->ctx, both event and ctx will be
		 * complete enough. See perf_install_in_context().
		 */
		if (!smp_load_acquire(&event->ctx))
			continue;

		if (event->state < PERF_EVENT_STATE_INACTIVE)
			continue;
		if (!event_filter_match(event))
			continue;
		output(event, data);
	}
}