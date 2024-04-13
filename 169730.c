__perf_event_output(struct perf_event *event,
		    struct perf_sample_data *data,
		    struct pt_regs *regs,
		    int (*output_begin)(struct perf_output_handle *,
					struct perf_event *,
					unsigned int))
{
	struct perf_output_handle handle;
	struct perf_event_header header;
	int err;

	/* protect the callchain buffers */
	rcu_read_lock();

	perf_prepare_sample(&header, data, event, regs);

	err = output_begin(&handle, event, header.size);
	if (err)
		goto exit;

	perf_output_sample(&handle, &header, data, event);

	perf_output_end(&handle);

exit:
	rcu_read_unlock();
	return err;
}