perf_cgroup_event_disable(struct perf_event *event, struct perf_event_context *ctx)
{
	struct perf_cpu_context *cpuctx;

	if (!is_cgroup_event(event))
		return;

	/*
	 * Because cgroup events are always per-cpu events,
	 * @ctx == &cpuctx->ctx.
	 */
	cpuctx = container_of(ctx, struct perf_cpu_context, ctx);

	if (--ctx->nr_cgroups)
		return;

	if (ctx->is_active && cpuctx->cgrp)
		cpuctx->cgrp = NULL;

	list_del(&cpuctx->cgrp_cpuctx_entry);
}