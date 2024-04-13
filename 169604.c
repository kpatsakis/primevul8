static inline u64 perf_event_count(struct perf_event *event)
{
	return local64_read(&event->count) + atomic64_read(&event->child_count);
}