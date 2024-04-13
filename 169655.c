perf_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct perf_event *event = file->private_data;
	struct perf_event_context *ctx;
	int ret;

	ret = security_perf_event_read(event);
	if (ret)
		return ret;

	ctx = perf_event_ctx_lock(event);
	ret = __perf_read(event, buf, count);
	perf_event_ctx_unlock(event, ctx);

	return ret;
}