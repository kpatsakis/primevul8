ctx_pinned_sched_in(struct perf_event_context *ctx,
		    struct perf_cpu_context *cpuctx)
{
	int can_add_hw = 1;

	if (ctx != &cpuctx->ctx)
		cpuctx = NULL;

	visit_groups_merge(cpuctx, &ctx->pinned_groups,
			   smp_processor_id(),
			   merge_sched_in, &can_add_hw);
}