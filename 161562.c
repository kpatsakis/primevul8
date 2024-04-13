bool mod_delayed_work_on(int cpu, struct workqueue_struct *wq,
			 struct delayed_work *dwork, unsigned long delay)
{
	unsigned long flags;
	int ret;

	do {
		ret = try_to_grab_pending(&dwork->work, true, &flags);
	} while (unlikely(ret == -EAGAIN));

	if (likely(ret >= 0)) {
		__queue_delayed_work(cpu, wq, dwork, delay);
		local_irq_restore(flags);
	}

	/* -ENOENT from try_to_grab_pending() becomes %true */
	return ret;
}