static int  __perf_install_in_context(void *info)
{
	struct perf_event *event = info;
	struct perf_event_context *ctx = event->ctx;
	struct perf_cpu_context *cpuctx = __get_cpu_context(ctx);
	struct perf_event_context *task_ctx = cpuctx->task_ctx;
	bool reprogram = true;
	int ret = 0;

	raw_spin_lock(&cpuctx->ctx.lock);
	if (ctx->task) {
		raw_spin_lock(&ctx->lock);
		task_ctx = ctx;

		reprogram = (ctx->task == current);

		/*
		 * If the task is running, it must be running on this CPU,
		 * otherwise we cannot reprogram things.
		 *
		 * If its not running, we don't care, ctx->lock will
		 * serialize against it becoming runnable.
		 */
		if (task_curr(ctx->task) && !reprogram) {
			ret = -ESRCH;
			goto unlock;
		}

		WARN_ON_ONCE(reprogram && cpuctx->task_ctx && cpuctx->task_ctx != ctx);
	} else if (task_ctx) {
		raw_spin_lock(&task_ctx->lock);
	}

#ifdef CONFIG_CGROUP_PERF
	if (event->state > PERF_EVENT_STATE_OFF && is_cgroup_event(event)) {
		/*
		 * If the current cgroup doesn't match the event's
		 * cgroup, we should not try to schedule it.
		 */
		struct perf_cgroup *cgrp = perf_cgroup_from_task(current, ctx);
		reprogram = cgroup_is_descendant(cgrp->css.cgroup,
					event->cgrp->css.cgroup);
	}
#endif

	if (reprogram) {
		ctx_sched_out(ctx, cpuctx, EVENT_TIME);
		add_event_to_ctx(event, ctx);
		ctx_resched(cpuctx, task_ctx, get_event_type(event));
	} else {
		add_event_to_ctx(event, ctx);
	}

unlock:
	perf_ctx_unlock(cpuctx, task_ctx);

	return ret;
}