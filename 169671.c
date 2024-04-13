event_filter_match(struct perf_event *event)
{
	return (event->cpu == -1 || event->cpu == smp_processor_id()) &&
	       perf_cgroup_match(event) && pmu_filter_match(event);
}