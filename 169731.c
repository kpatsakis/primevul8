perf_event_groups_delete(struct perf_event_groups *groups,
			 struct perf_event *event)
{
	WARN_ON_ONCE(RB_EMPTY_NODE(&event->group_node) ||
		     RB_EMPTY_ROOT(&groups->tree));

	rb_erase(&event->group_node, &groups->tree);
	init_event_group(event);
}