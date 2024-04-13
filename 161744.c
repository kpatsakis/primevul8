static int workqueue_apply_unbound_cpumask(void)
{
	LIST_HEAD(ctxs);
	int ret = 0;
	struct workqueue_struct *wq;
	struct apply_wqattrs_ctx *ctx, *n;

	lockdep_assert_held(&wq_pool_mutex);

	list_for_each_entry(wq, &workqueues, list) {
		if (!(wq->flags & WQ_UNBOUND))
			continue;
		/* creating multiple pwqs breaks ordering guarantee */
		if (wq->flags & __WQ_ORDERED)
			continue;

		ctx = apply_wqattrs_prepare(wq, wq->unbound_attrs);
		if (!ctx) {
			ret = -ENOMEM;
			break;
		}

		list_add_tail(&ctx->list, &ctxs);
	}

	list_for_each_entry_safe(ctx, n, &ctxs, list) {
		if (!ret)
			apply_wqattrs_commit(ctx);
		apply_wqattrs_cleanup(ctx);
	}

	return ret;
}