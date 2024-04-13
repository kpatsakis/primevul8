__perf_update_times(struct perf_event *event, u64 now, u64 *enabled, u64 *running)
{
	enum perf_event_state state = __perf_effective_state(event);
	u64 delta = now - event->tstamp;

	*enabled = event->total_time_enabled;
	if (state >= PERF_EVENT_STATE_INACTIVE)
		*enabled += delta;

	*running = event->total_time_running;
	if (state >= PERF_EVENT_STATE_ACTIVE)
		*running += delta;
}