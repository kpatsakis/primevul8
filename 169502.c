static void perf_event_task(struct task_struct *task,
			      struct perf_event_context *task_ctx,
			      int new)
{
	struct perf_task_event task_event;

	if (!atomic_read(&nr_comm_events) &&
	    !atomic_read(&nr_mmap_events) &&
	    !atomic_read(&nr_task_events))
		return;

	task_event = (struct perf_task_event){
		.task	  = task,
		.task_ctx = task_ctx,
		.event_id    = {
			.header = {
				.type = new ? PERF_RECORD_FORK : PERF_RECORD_EXIT,
				.misc = 0,
				.size = sizeof(task_event.event_id),
			},
			/* .pid  */
			/* .ppid */
			/* .tid  */
			/* .ptid */
			/* .time */
		},
	};

	perf_iterate_sb(perf_event_task_output,
		       &task_event,
		       task_ctx);
}