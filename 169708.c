perf_cgroup_event_enable(struct perf_event *event, struct perf_event_context *ctx)
{
	struct perf_cpu_context *cpuctx;

	if (!is_cgroup_event(event))
		return;

	/*
	 * Because cgroup events are always per-cpu events,
	 * @ctx == &cpuctx->ctx.
	 */
	cpuctx = container_of(ctx, struct perf_cpu_context, ctx);

	/*
	 * Since setting cpuctx->cgrp is conditional on the current @cgrp
	 * matching the event's cgroup, we must do this for every new event,
	 * because if the first would mismatch, the second would not try again
	 * and we would leave cpuctx->cgrp unset.
	 */
	if (ctx->is_active && !cpuctx->cgrp) {
		struct perf_cgroup *cgrp = perf_cgroup_from_task(current, ctx);

		if (cgroup_is_descendant(cgrp->css.cgroup, event->cgrp->css.cgroup))
			cpuctx->cgrp = cgrp;
	}

	if (ctx->nr_cgroups++)
		return;

	list_add(&cpuctx->cgrp_cpuctx_entry,
			per_cpu_ptr(&cgrp_cpuctx_list, event->cpu));
}