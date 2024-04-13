ttwu_do_activate(struct rq *rq, struct task_struct *p, int wake_flags)
{
	lockdep_assert_held(&rq->lock);

#ifdef CONFIG_SMP
	if (p->sched_contributes_to_load)
		rq->nr_uninterruptible--;
#endif

	ttwu_activate(rq, p, ENQUEUE_WAKEUP | ENQUEUE_WAKING);
	ttwu_do_wakeup(rq, p, wake_flags);
}