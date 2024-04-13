perf_callchain(struct perf_event *event, struct pt_regs *regs)
{
	bool kernel = !event->attr.exclude_callchain_kernel;
	bool user   = !event->attr.exclude_callchain_user;
	/* Disallow cross-task user callchains. */
	bool crosstask = event->ctx->task && event->ctx->task != current;
	const u32 max_stack = event->attr.sample_max_stack;
	struct perf_callchain_entry *callchain;

	if (!kernel && !user)
		return &__empty_callchain;

	callchain = get_perf_callchain(regs, 0, kernel, user,
				       max_stack, crosstask, true);
	return callchain ?: &__empty_callchain;
}