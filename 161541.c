__acquires(&pool->lock)
{
	struct pool_workqueue *pwq = get_work_pwq(work);
	struct worker_pool *pool = worker->pool;
	bool cpu_intensive = pwq->wq->flags & WQ_CPU_INTENSIVE;
	int work_color;
	struct worker *collision;
#ifdef CONFIG_LOCKDEP
	/*
	 * It is permissible to free the struct work_struct from
	 * inside the function that is called from it, this we need to
	 * take into account for lockdep too.  To avoid bogus "held
	 * lock freed" warnings as well as problems when looking into
	 * work->lockdep_map, make a copy and use that here.
	 */
	struct lockdep_map lockdep_map;

	lockdep_copy_map(&lockdep_map, &work->lockdep_map);
#endif
	/* ensure we're on the correct CPU */
	WARN_ON_ONCE(!(pool->flags & POOL_DISASSOCIATED) &&
		     raw_smp_processor_id() != pool->cpu);

	/*
	 * A single work shouldn't be executed concurrently by
	 * multiple workers on a single cpu.  Check whether anyone is
	 * already processing the work.  If so, defer the work to the
	 * currently executing one.
	 */
	collision = find_worker_executing_work(pool, work);
	if (unlikely(collision)) {
		move_linked_works(work, &collision->scheduled, NULL);
		return;
	}

	/* claim and dequeue */
	debug_work_deactivate(work);
	hash_add(pool->busy_hash, &worker->hentry, (unsigned long)work);
	worker->current_work = work;
	worker->current_func = work->func;
	worker->current_pwq = pwq;
	work_color = get_work_color(work);

	list_del_init(&work->entry);

	/*
	 * CPU intensive works don't participate in concurrency management.
	 * They're the scheduler's responsibility.  This takes @worker out
	 * of concurrency management and the next code block will chain
	 * execution of the pending work items.
	 */
	if (unlikely(cpu_intensive))
		worker_set_flags(worker, WORKER_CPU_INTENSIVE);

	/*
	 * Wake up another worker if necessary.  The condition is always
	 * false for normal per-cpu workers since nr_running would always
	 * be >= 1 at this point.  This is used to chain execution of the
	 * pending work items for WORKER_NOT_RUNNING workers such as the
	 * UNBOUND and CPU_INTENSIVE ones.
	 */
	if (need_more_worker(pool))
		wake_up_worker(pool);

	/*
	 * Record the last pool and clear PENDING which should be the last
	 * update to @work.  Also, do this inside @pool->lock so that
	 * PENDING and queued state changes happen together while IRQ is
	 * disabled.
	 */
	set_work_pool_and_clear_pending(work, pool->id);

	spin_unlock_irq(&pool->lock);

	lock_map_acquire_read(&pwq->wq->lockdep_map);
	lock_map_acquire(&lockdep_map);
	trace_workqueue_execute_start(work);
	worker->current_func(work);
	/*
	 * While we must be careful to not use "work" after this, the trace
	 * point will only record its address.
	 */
	trace_workqueue_execute_end(work);
	lock_map_release(&lockdep_map);
	lock_map_release(&pwq->wq->lockdep_map);

	if (unlikely(in_atomic() || lockdep_depth(current) > 0)) {
		pr_err("BUG: workqueue leaked lock or atomic: %s/0x%08x/%d\n"
		       "     last function: %pf\n",
		       current->comm, preempt_count(), task_pid_nr(current),
		       worker->current_func);
		debug_show_held_locks(current);
		dump_stack();
	}

	/*
	 * The following prevents a kworker from hogging CPU on !PREEMPT
	 * kernels, where a requeueing work item waiting for something to
	 * happen could deadlock with stop_machine as such work item could
	 * indefinitely requeue itself while all other CPUs are trapped in
	 * stop_machine. At the same time, report a quiescent RCU state so
	 * the same condition doesn't freeze RCU.
	 */
	cond_resched_rcu_qs();

	spin_lock_irq(&pool->lock);

	/* clear cpu intensive status */
	if (unlikely(cpu_intensive))
		worker_clr_flags(worker, WORKER_CPU_INTENSIVE);

	/* we're done with it, release */
	hash_del(&worker->hentry);
	worker->current_work = NULL;
	worker->current_func = NULL;
	worker->current_pwq = NULL;
	worker->desc_valid = false;
	pwq_dec_nr_in_flight(pwq, work_color);
}