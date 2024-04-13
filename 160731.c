static void report_progress(
	checkout_data *data,
	const char *path)
{
	if (data->opts.progress_cb)
		data->opts.progress_cb(
			path, data->completed_steps, data->total_steps,
			data->opts.progress_payload);
}