perf_event_create_kernel_counter(struct perf_event_attr *attr, int cpu,
				 struct task_struct *task,
				 perf_overflow_handler_t overflow_handler,
				 void *context)
{
	struct perf_event_context *ctx;
	struct perf_event *event;
	int err;

	/*
	 * Grouping is not supported for kernel events, neither is 'AUX',
	 * make sure the caller's intentions are adjusted.
	 */
	if (attr->aux_output)
		return ERR_PTR(-EINVAL);

	event = perf_event_alloc(attr, cpu, task, NULL, NULL,
				 overflow_handler, context, -1);
	if (IS_ERR(event)) {
		err = PTR_ERR(event);
		goto err;
	}

	/* Mark owner so we could distinguish it from user events. */
	event->owner = TASK_TOMBSTONE;

	/*
	 * Get the target context (task or percpu):
	 */
	ctx = find_get_context(event->pmu, task, event);
	if (IS_ERR(ctx)) {
		err = PTR_ERR(ctx);
		goto err_free;
	}

	WARN_ON_ONCE(ctx->parent_ctx);
	mutex_lock(&ctx->mutex);
	if (ctx->task == TASK_TOMBSTONE) {
		err = -ESRCH;
		goto err_unlock;
	}

	if (!task) {
		/*
		 * Check if the @cpu we're creating an event for is online.
		 *
		 * We use the perf_cpu_context::ctx::mutex to serialize against
		 * the hotplug notifiers. See perf_event_{init,exit}_cpu().
		 */
		struct perf_cpu_context *cpuctx =
			container_of(ctx, struct perf_cpu_context, ctx);
		if (!cpuctx->online) {
			err = -ENODEV;
			goto err_unlock;
		}
	}

	if (!exclusive_event_installable(event, ctx)) {
		err = -EBUSY;
		goto err_unlock;
	}

	perf_install_in_context(ctx, event, event->cpu);
	perf_unpin_context(ctx);
	mutex_unlock(&ctx->mutex);

	return event;

err_unlock:
	mutex_unlock(&ctx->mutex);
	perf_unpin_context(ctx);
	put_ctx(ctx);
err_free:
	free_event(event);
err:
	return ERR_PTR(err);
}