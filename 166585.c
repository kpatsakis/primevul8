static void filter_disable(struct trace_event_file *file)
{
	unsigned long old_flags = file->flags;

	file->flags &= ~EVENT_FILE_FL_FILTERED;

	if (old_flags != file->flags)
		trace_buffered_event_disable();
}