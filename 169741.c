event_sched_in(struct perf_event *event,
		 struct perf_cpu_context *cpuctx,
		 struct perf_event_context *ctx)
{
	int ret = 0;

	WARN_ON_ONCE(event->ctx != ctx);

	lockdep_assert_held(&ctx->lock);

	if (event->state <= PERF_EVENT_STATE_OFF)
		return 0;

	WRITE_ONCE(event->oncpu, smp_processor_id());
	/*
	 * Order event::oncpu write to happen before the ACTIVE state is
	 * visible. This allows perf_event_{stop,read}() to observe the correct
	 * ->oncpu if it sees ACTIVE.
	 */
	smp_wmb();
	perf_event_set_state(event, PERF_EVENT_STATE_ACTIVE);

	/*
	 * Unthrottle events, since we scheduled we might have missed several
	 * ticks already, also for a heavily scheduling task there is little
	 * guarantee it'll get a tick in a timely manner.
	 */
	if (unlikely(event->hw.interrupts == MAX_INTERRUPTS)) {
		perf_log_throttle(event, 1);
		event->hw.interrupts = 0;
	}

	perf_pmu_disable(event->pmu);

	perf_set_shadow_time(event, ctx);

	perf_log_itrace_start(event);

	if (event->pmu->add(event, PERF_EF_START)) {
		perf_event_set_state(event, PERF_EVENT_STATE_INACTIVE);
		event->oncpu = -1;
		ret = -EAGAIN;
		goto out;
	}

	if (!is_software_event(event))
		cpuctx->active_oncpu++;
	if (!ctx->nr_active++)
		perf_event_ctx_activate(ctx);
	if (event->attr.freq && event->attr.sample_freq)
		ctx->nr_freq++;

	if (event->attr.exclusive)
		cpuctx->exclusive = 1;

out:
	perf_pmu_enable(event->pmu);

	return ret;
}