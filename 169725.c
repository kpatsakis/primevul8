list_del_event(struct perf_event *event, struct perf_event_context *ctx)
{
	WARN_ON_ONCE(event->ctx != ctx);
	lockdep_assert_held(&ctx->lock);

	/*
	 * We can have double detach due to exit/hot-unplug + close.
	 */
	if (!(event->attach_state & PERF_ATTACH_CONTEXT))
		return;

	event->attach_state &= ~PERF_ATTACH_CONTEXT;

	ctx->nr_events--;
	if (event->attr.inherit_stat)
		ctx->nr_stat--;

	list_del_rcu(&event->event_entry);

	if (event->group_leader == event)
		del_event_from_groups(event, ctx);

	/*
	 * If event was in error state, then keep it
	 * that way, otherwise bogus counts will be
	 * returned on read(). The only way to get out
	 * of error state is by explicit re-enabling
	 * of the event
	 */
	if (event->state > PERF_EVENT_STATE_OFF) {
		perf_cgroup_event_disable(event, ctx);
		perf_event_set_state(event, PERF_EVENT_STATE_OFF);
	}

	ctx->generation++;
}