perf_event_output(struct perf_event *event,
		  struct perf_sample_data *data,
		  struct pt_regs *regs)
{
	return __perf_event_output(event, data, regs, perf_output_begin);
}