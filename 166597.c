event_no_set_filter_flag(struct trace_event_file *file)
{
	if (file->flags & EVENT_FILE_FL_NO_SET_FILTER)
		return true;

	return false;
}