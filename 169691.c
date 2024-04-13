__perf_event_ctx_lock_double(struct perf_event *group_leader,
			     struct perf_event_context *ctx)
{
	struct perf_event_context *gctx;

again:
	rcu_read_lock();
	gctx = READ_ONCE(group_leader->ctx);
	if (!refcount_inc_not_zero(&gctx->refcount)) {
		rcu_read_unlock();
		goto again;
	}
	rcu_read_unlock();

	mutex_lock_double(&gctx->mutex, &ctx->mutex);

	if (group_leader->ctx != gctx) {
		mutex_unlock(&ctx->mutex);
		mutex_unlock(&gctx->mutex);
		put_ctx(gctx);
		goto again;
	}

	return gctx;
}