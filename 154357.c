__setparam_dl(struct task_struct *p, const struct sched_attr *attr)
{
	struct sched_dl_entity *dl_se = &p->dl;

	dl_se->dl_runtime = attr->sched_runtime;
	dl_se->dl_deadline = attr->sched_deadline;
	dl_se->dl_period = attr->sched_period ?: dl_se->dl_deadline;
	dl_se->flags = attr->sched_flags;
	dl_se->dl_bw = to_ratio(dl_se->dl_period, dl_se->dl_runtime);

	/*
	 * Changing the parameters of a task is 'tricky' and we're not doing
	 * the correct thing -- also see task_dead_dl() and switched_from_dl().
	 *
	 * What we SHOULD do is delay the bandwidth release until the 0-lag
	 * point. This would include retaining the task_struct until that time
	 * and change dl_overflow() to not immediately decrement the current
	 * amount.
	 *
	 * Instead we retain the current runtime/deadline and let the new
	 * parameters take effect after the current reservation period lapses.
	 * This is safe (albeit pessimistic) because the 0-lag point is always
	 * before the current scheduling deadline.
	 *
	 * We can still have temporary overloads because we do not delay the
	 * change in bandwidth until that time; so admission control is
	 * not on the safe side. It does however guarantee tasks will never
	 * consume more than promised.
	 */
}