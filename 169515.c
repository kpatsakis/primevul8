ctx_event_to_rotate(struct perf_event_context *ctx)
{
	struct perf_event *event;

	/* pick the first active flexible event */
	event = list_first_entry_or_null(&ctx->flexible_active,
					 struct perf_event, active_list);

	/* if no active flexible event, pick the first event */
	if (!event) {
		event = rb_entry_safe(rb_first(&ctx->flexible_groups.tree),
				      typeof(*event), group_node);
	}

	/*
	 * Unconditionally clear rotate_necessary; if ctx_flexible_sched_in()
	 * finds there are unschedulable events, it will set it again.
	 */
	ctx->rotate_necessary = 0;

	return event;
}