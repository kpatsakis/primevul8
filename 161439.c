static void check_flush_dependency(struct workqueue_struct *target_wq,
				   struct work_struct *target_work)
{
	work_func_t target_func = target_work ? target_work->func : NULL;
	struct worker *worker;

	if (target_wq->flags & WQ_MEM_RECLAIM)
		return;

	worker = current_wq_worker();

	WARN_ONCE(current->flags & PF_MEMALLOC,
		  "workqueue: PF_MEMALLOC task %d(%s) is flushing !WQ_MEM_RECLAIM %s:%pf",
		  current->pid, current->comm, target_wq->name, target_func);
	WARN_ONCE(worker && ((worker->current_pwq->wq->flags &
			      (WQ_MEM_RECLAIM | __WQ_LEGACY)) == WQ_MEM_RECLAIM),
		  "workqueue: WQ_MEM_RECLAIM %s:%pf is flushing !WQ_MEM_RECLAIM %s:%pf",
		  worker->current_pwq->wq->name, worker->current_func,
		  target_wq->name, target_func);
}