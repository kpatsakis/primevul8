static void perf_set_shadow_time(struct perf_event *event,
				 struct perf_event_context *ctx)
{
	/*
	 * use the correct time source for the time snapshot
	 *
	 * We could get by without this by leveraging the
	 * fact that to get to this function, the caller
	 * has most likely already called update_context_time()
	 * and update_cgrp_time_xx() and thus both timestamp
	 * are identical (or very close). Given that tstamp is,
	 * already adjusted for cgroup, we could say that:
	 *    tstamp - ctx->timestamp
	 * is equivalent to
	 *    tstamp - cgrp->timestamp.
	 *
	 * Then, in perf_output_read(), the calculation would
	 * work with no changes because:
	 * - event is guaranteed scheduled in
	 * - no scheduled out in between
	 * - thus the timestamp would be the same
	 *
	 * But this is a bit hairy.
	 *
	 * So instead, we have an explicit cgroup call to remain
	 * within the time time source all along. We believe it
	 * is cleaner and simpler to understand.
	 */
	if (is_cgroup_event(event))
		perf_cgroup_set_shadow_time(event, event->tstamp);
	else
		event->shadow_ctx_time = event->tstamp - ctx->timestamp;
}