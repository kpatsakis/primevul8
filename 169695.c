event_sched_out(struct perf_event *event,
		  struct perf_cpu_context *cpuctx,
		  struct perf_event_context *ctx)
{
	enum perf_event_state state = PERF_EVENT_STATE_INACTIVE;

	WARN_ON_ONCE(event->ctx != ctx);
	lockdep_assert_held(&ctx->lock);

	if (event->state != PERF_EVENT_STATE_ACTIVE)
		return;

	/*
	 * Asymmetry; we only schedule events _IN_ through ctx_sched_in(), but
	 * we can schedule events _OUT_ individually through things like
	 * __perf_remove_from_context().
	 */
	list_del_init(&event->active_list);

	perf_pmu_disable(event->pmu);

	event->pmu->del(event, 0);
	event->oncpu = -1;

	if (READ_ONCE(event->pending_disable) >= 0) {
		WRITE_ONCE(event->pending_disable, -1);
		perf_cgroup_event_disable(event, ctx);
		state = PERF_EVENT_STATE_OFF;
	}
	perf_event_set_state(event, state);

	if (!is_software_event(event))
		cpuctx->active_oncpu--;
	if (!--ctx->nr_active)
		perf_event_ctx_deactivate(ctx);
	if (event->attr.freq && event->attr.sample_freq)
		ctx->nr_freq--;
	if (event->attr.exclusive || !cpuctx->active_oncpu)
		cpuctx->exclusive = 0;

	perf_pmu_enable(event->pmu);
}