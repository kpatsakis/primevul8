__perf_effective_state(struct perf_event *event)
{
	struct perf_event *leader = event->group_leader;

	if (leader->state <= PERF_EVENT_STATE_OFF)
		return leader->state;

	return event->state;
}