
static int __io_sq_thread(struct io_ring_ctx *ctx, bool cap_entries)
{
	unsigned int to_submit;
	int ret = 0;

	to_submit = io_sqring_entries(ctx);
	/* if we're handling multiple rings, cap submit size for fairness */
	if (cap_entries && to_submit > IORING_SQPOLL_CAP_ENTRIES_VALUE)
		to_submit = IORING_SQPOLL_CAP_ENTRIES_VALUE;

	if (!wq_list_empty(&ctx->iopoll_list) || to_submit) {
		const struct cred *creds = NULL;

		if (ctx->sq_creds != current_cred())
			creds = override_creds(ctx->sq_creds);

		mutex_lock(&ctx->uring_lock);
		if (!wq_list_empty(&ctx->iopoll_list))
			io_do_iopoll(ctx, true);

		/*
		 * Don't submit if refs are dying, good for io_uring_register(),
		 * but also it is relied upon by io_ring_exit_work()
		 */
		if (to_submit && likely(!percpu_ref_is_dying(&ctx->refs)) &&
		    !(ctx->flags & IORING_SETUP_R_DISABLED))
			ret = io_submit_sqes(ctx, to_submit);
		mutex_unlock(&ctx->uring_lock);

		if (to_submit && wq_has_sleeper(&ctx->sqo_sq_wait))
			wake_up(&ctx->sqo_sq_wait);
		if (creds)
			revert_creds(creds);
	}

	return ret;