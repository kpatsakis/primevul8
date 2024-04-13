static void tctx_task_work(struct callback_head *cb)
{
	bool uring_locked = false;
	struct io_ring_ctx *ctx = NULL;
	struct io_uring_task *tctx = container_of(cb, struct io_uring_task,
						  task_work);

	while (1) {
		struct io_wq_work_node *node1, *node2;

		if (!tctx->task_list.first &&
		    !tctx->prior_task_list.first && uring_locked)
			io_submit_flush_completions(ctx);

		spin_lock_irq(&tctx->task_lock);
		node1 = tctx->prior_task_list.first;
		node2 = tctx->task_list.first;
		INIT_WQ_LIST(&tctx->task_list);
		INIT_WQ_LIST(&tctx->prior_task_list);
		if (!node2 && !node1)
			tctx->task_running = false;
		spin_unlock_irq(&tctx->task_lock);
		if (!node2 && !node1)
			break;

		if (node1)
			handle_prev_tw_list(node1, &ctx, &uring_locked);

		if (node2)
			handle_tw_list(node2, &ctx, &uring_locked);
		cond_resched();
	}

	ctx_flush_and_put(ctx, &uring_locked);

	/* relaxed read is enough as only the task itself sets ->in_idle */
	if (unlikely(atomic_read(&tctx->in_idle)))
		io_uring_drop_tctx_refs(current);
}