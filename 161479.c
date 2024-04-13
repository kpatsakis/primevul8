static void process_scheduled_works(struct worker *worker)
{
	while (!list_empty(&worker->scheduled)) {
		struct work_struct *work = list_first_entry(&worker->scheduled,
						struct work_struct, entry);
		process_one_work(worker, work);
	}
}