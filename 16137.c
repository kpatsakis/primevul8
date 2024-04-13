/* Returns true if we found and killed one or more timeouts */
static __cold bool io_kill_timeouts(struct io_ring_ctx *ctx,
				    struct task_struct *tsk, bool cancel_all)
{
	struct io_kiocb *req, *tmp;
	int canceled = 0;

	spin_lock(&ctx->completion_lock);
	spin_lock_irq(&ctx->timeout_lock);
	list_for_each_entry_safe(req, tmp, &ctx->timeout_list, timeout.list) {
		if (io_match_task(req, tsk, cancel_all)) {
			io_kill_timeout(req, -ECANCELED);
			canceled++;
		}
	}
	spin_unlock_irq(&ctx->timeout_lock);
	if (canceled != 0)
		io_commit_cqring(ctx);
	spin_unlock(&ctx->completion_lock);
	if (canceled != 0)
		io_cqring_ev_posted(ctx);
	return canceled != 0;