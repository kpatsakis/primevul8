add_event_to_groups(struct perf_event *event, struct perf_event_context *ctx)
{
	struct perf_event_groups *groups;

	groups = get_event_groups(event, ctx);
	perf_event_groups_insert(groups, event);
}