group_sched_in(struct perf_event *group_event,
	       struct perf_cpu_context *cpuctx,
	       struct perf_event_context *ctx)
{
	struct perf_event *event, *partial_group = NULL;
	struct pmu *pmu = ctx->pmu;

	if (group_event->state == PERF_EVENT_STATE_OFF)
		return 0;

	pmu->start_txn(pmu, PERF_PMU_TXN_ADD);

	if (event_sched_in(group_event, cpuctx, ctx)) {
		pmu->cancel_txn(pmu);
		perf_mux_hrtimer_restart(cpuctx);
		return -EAGAIN;
	}

	/*
	 * Schedule in siblings as one group (if any):
	 */
	for_each_sibling_event(event, group_event) {
		if (event_sched_in(event, cpuctx, ctx)) {
			partial_group = event;
			goto group_error;
		}
	}

	if (!pmu->commit_txn(pmu))
		return 0;

group_error:
	/*
	 * Groups can be scheduled in as one unit only, so undo any
	 * partial group before returning:
	 * The events up to the failed event are scheduled out normally.
	 */
	for_each_sibling_event(event, group_event) {
		if (event == partial_group)
			break;

		event_sched_out(event, cpuctx, ctx);
	}
	event_sched_out(group_event, cpuctx, ctx);

	pmu->cancel_txn(pmu);

	perf_mux_hrtimer_restart(cpuctx);

	return -EAGAIN;
}