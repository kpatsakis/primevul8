static void perf_pmu_output_stop(struct perf_event *event)
{
	struct perf_event *iter;
	int err, cpu;

restart:
	rcu_read_lock();
	list_for_each_entry_rcu(iter, &event->rb->event_list, rb_entry) {
		/*
		 * For per-CPU events, we need to make sure that neither they
		 * nor their children are running; for cpu==-1 events it's
		 * sufficient to stop the event itself if it's active, since
		 * it can't have children.
		 */
		cpu = iter->cpu;
		if (cpu == -1)
			cpu = READ_ONCE(iter->oncpu);

		if (cpu == -1)
			continue;

		err = cpu_function_call(cpu, __perf_pmu_output_stop, event);
		if (err == -EAGAIN) {
			rcu_read_unlock();
			goto restart;
		}
	}
	rcu_read_unlock();
}