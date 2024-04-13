try_to_wake_up(struct task_struct *p, unsigned int state, int wake_flags)
{
	unsigned long flags;
	int cpu, success = 0;

	/*
	 * If we are going to wake up a thread waiting for CONDITION we
	 * need to ensure that CONDITION=1 done by the caller can not be
	 * reordered with p->state check below. This pairs with mb() in
	 * set_current_state() the waiting thread does.
	 */
	smp_mb__before_spinlock();
	raw_spin_lock_irqsave(&p->pi_lock, flags);
	if (!(p->state & state))
		goto out;

	trace_sched_waking(p);

	success = 1; /* we're going to change ->state */
	cpu = task_cpu(p);

	if (p->on_rq && ttwu_remote(p, wake_flags))
		goto stat;

#ifdef CONFIG_SMP
	/*
	 * Ensure we load p->on_cpu _after_ p->on_rq, otherwise it would be
	 * possible to, falsely, observe p->on_cpu == 0.
	 *
	 * One must be running (->on_cpu == 1) in order to remove oneself
	 * from the runqueue.
	 *
	 *  [S] ->on_cpu = 1;	[L] ->on_rq
	 *      UNLOCK rq->lock
	 *			RMB
	 *      LOCK   rq->lock
	 *  [S] ->on_rq = 0;    [L] ->on_cpu
	 *
	 * Pairs with the full barrier implied in the UNLOCK+LOCK on rq->lock
	 * from the consecutive calls to schedule(); the first switching to our
	 * task, the second putting it to sleep.
	 */
	smp_rmb();

	/*
	 * If the owning (remote) cpu is still in the middle of schedule() with
	 * this task as prev, wait until its done referencing the task.
	 *
	 * Pairs with the smp_store_release() in finish_lock_switch().
	 *
	 * This ensures that tasks getting woken will be fully ordered against
	 * their previous state and preserve Program Order.
	 */
	smp_cond_acquire(!p->on_cpu);

	p->sched_contributes_to_load = !!task_contributes_to_load(p);
	p->state = TASK_WAKING;

	if (p->sched_class->task_waking)
		p->sched_class->task_waking(p);

	cpu = select_task_rq(p, p->wake_cpu, SD_BALANCE_WAKE, wake_flags);
	if (task_cpu(p) != cpu) {
		wake_flags |= WF_MIGRATED;
		set_task_cpu(p, cpu);
	}
#endif /* CONFIG_SMP */

	ttwu_queue(p, cpu);
stat:
	if (schedstat_enabled())
		ttwu_stat(p, cpu, wake_flags);
out:
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

	return success;
}