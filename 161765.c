bool flush_work(struct work_struct *work)
{
	struct wq_barrier barr;

	if (WARN_ON(!wq_online))
		return false;

	lock_map_acquire(&work->lockdep_map);
	lock_map_release(&work->lockdep_map);

	if (start_flush_work(work, &barr)) {
		wait_for_completion(&barr.done);
		destroy_work_on_stack(&barr.work);
		return true;
	} else {
		return false;
	}
}