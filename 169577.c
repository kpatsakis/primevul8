perf_event_set_addr_filter(struct perf_event *event, char *filter_str)
{
	LIST_HEAD(filters);
	int ret;

	/*
	 * Since this is called in perf_ioctl() path, we're already holding
	 * ctx::mutex.
	 */
	lockdep_assert_held(&event->ctx->mutex);

	if (WARN_ON_ONCE(event->parent))
		return -EINVAL;

	ret = perf_event_parse_addr_filter(event, filter_str, &filters);
	if (ret)
		goto fail_clear_files;

	ret = event->pmu->addr_filters_validate(&filters);
	if (ret)
		goto fail_free_filters;

	/* remove existing filters, if any */
	perf_addr_filters_splice(event, &filters);

	/* install new filters */
	perf_event_for_each_child(event, perf_event_addr_filters_apply);

	return ret;

fail_free_filters:
	free_filters_list(&filters);

fail_clear_files:
	event->addr_filters.nr_file_filters = 0;

	return ret;
}