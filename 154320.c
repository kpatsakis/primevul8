ttwu_do_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)
{
	check_preempt_curr(rq, p, wake_flags);
	p->state = TASK_RUNNING;
	trace_sched_wakeup(p);

#ifdef CONFIG_SMP
	if (p->sched_class->task_woken) {
		/*
		 * Our task @p is fully woken up and running; so its safe to
		 * drop the rq->lock, hereafter rq is only used for statistics.
		 */
		lockdep_unpin_lock(&rq->lock);
		p->sched_class->task_woken(rq, p);
		lockdep_pin_lock(&rq->lock);
	}

	if (rq->idle_stamp) {
		u64 delta = rq_clock(rq) - rq->idle_stamp;
		u64 max = 2*rq->max_idle_balance_cost;

		update_avg(&rq->avg_idle, delta);

		if (rq->avg_idle > max)
			rq->avg_idle = max;

		rq->idle_stamp = 0;
	}
#endif
}