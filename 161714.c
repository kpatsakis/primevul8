static void insert_wq_barrier(struct pool_workqueue *pwq,
			      struct wq_barrier *barr,
			      struct work_struct *target, struct worker *worker)
{
	struct list_head *head;
	unsigned int linked = 0;

	/*
	 * debugobject calls are safe here even with pool->lock locked
	 * as we know for sure that this will not trigger any of the
	 * checks and call back into the fixup functions where we
	 * might deadlock.
	 */
	INIT_WORK_ONSTACK(&barr->work, wq_barrier_func);
	__set_bit(WORK_STRUCT_PENDING_BIT, work_data_bits(&barr->work));
	init_completion(&barr->done);
	barr->task = current;

	/*
	 * If @target is currently being executed, schedule the
	 * barrier to the worker; otherwise, put it after @target.
	 */
	if (worker)
		head = worker->scheduled.next;
	else {
		unsigned long *bits = work_data_bits(target);

		head = target->entry.next;
		/* there can already be other linked works, inherit and set */
		linked = *bits & WORK_STRUCT_LINKED;
		__set_bit(WORK_STRUCT_LINKED_BIT, bits);
	}

	debug_work_activate(&barr->work);
	insert_work(pwq, &barr->work, head,
		    work_color_to_flags(WORK_NO_COLOR) | linked);
}