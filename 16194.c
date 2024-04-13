static inline void io_put_task(struct task_struct *task, int nr)
{
	struct io_uring_task *tctx = task->io_uring;

	if (likely(task == current)) {
		tctx->cached_refs += nr;
	} else {
		percpu_counter_sub(&tctx->inflight, nr);
		if (unlikely(atomic_read(&tctx->in_idle)))
			wake_up(&tctx->wait);
		put_task_struct_many(task, nr);
	}
}