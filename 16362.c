 */
static __cold void io_uring_cancel_generic(bool cancel_all,
					   struct io_sq_data *sqd)
{
	struct io_uring_task *tctx = current->io_uring;
	struct io_ring_ctx *ctx;
	s64 inflight;
	DEFINE_WAIT(wait);

	WARN_ON_ONCE(sqd && sqd->thread != current);

	if (!current->io_uring)
		return;
	if (tctx->io_wq)
		io_wq_exit_start(tctx->io_wq);

	atomic_inc(&tctx->in_idle);
	do {
		io_uring_drop_tctx_refs(current);
		/* read completions before cancelations */
		inflight = tctx_inflight(tctx, !cancel_all);
		if (!inflight)
			break;

		if (!sqd) {
			struct io_tctx_node *node;
			unsigned long index;

			xa_for_each(&tctx->xa, index, node) {
				/* sqpoll task will cancel all its requests */
				if (node->ctx->sq_data)
					continue;
				io_uring_try_cancel_requests(node->ctx, current,
							     cancel_all);
			}
		} else {
			list_for_each_entry(ctx, &sqd->ctx_list, sqd_list)
				io_uring_try_cancel_requests(ctx, current,
							     cancel_all);
		}

		prepare_to_wait(&tctx->wait, &wait, TASK_INTERRUPTIBLE);
		io_run_task_work();
		io_uring_drop_tctx_refs(current);

		/*
		 * If we've seen completions, retry without waiting. This
		 * avoids a race where a completion comes in before we did
		 * prepare_to_wait().
		 */
		if (inflight == tctx_inflight(tctx, !cancel_all))
			schedule();
		finish_wait(&tctx->wait, &wait);
	} while (1);

	io_uring_clean_tctx(tctx);
	if (cancel_all) {
		/*
		 * We shouldn't run task_works after cancel, so just leave
		 * ->in_idle set for normal exit.
		 */
		atomic_dec(&tctx->in_idle);
		/* for exec all current's requests should be gone, kill tctx */
		__io_uring_free(current);
	}