pick_next_task(struct rq *rq, struct task_struct *prev)
{
	const struct sched_class *class = &fair_sched_class;
	struct task_struct *p;

	/*
	 * Optimization: we know that if all tasks are in
	 * the fair class we can call that function directly:
	 */
	if (likely(prev->sched_class == class &&
		   rq->nr_running == rq->cfs.h_nr_running)) {
		p = fair_sched_class.pick_next_task(rq, prev);
		if (unlikely(p == RETRY_TASK))
			goto again;

		/* assumes fair_sched_class->next == idle_sched_class */
		if (unlikely(!p))
			p = idle_sched_class.pick_next_task(rq, prev);

		return p;
	}

again:
	for_each_class(class) {
		p = class->pick_next_task(rq, prev);
		if (p) {
			if (unlikely(p == RETRY_TASK))
				goto again;
			return p;
		}
	}

	BUG(); /* the idle class will always have a runnable task */
}