static bool is_chained_work(struct workqueue_struct *wq)
{
	struct worker *worker;

	worker = current_wq_worker();
	/*
	 * Return %true iff I'm a worker execuing a work item on @wq.  If
	 * I'm @worker, it's safe to dereference it without locking.
	 */
	return worker && worker->current_pwq->wq == wq;
}