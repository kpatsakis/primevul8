static int apply_workqueue_attrs_locked(struct workqueue_struct *wq,
					const struct workqueue_attrs *attrs)
{
	struct apply_wqattrs_ctx *ctx;

	/* only unbound workqueues can change attributes */
	if (WARN_ON(!(wq->flags & WQ_UNBOUND)))
		return -EINVAL;

	/* creating multiple pwqs breaks ordering guarantee */
	if (WARN_ON((wq->flags & __WQ_ORDERED) && !list_empty(&wq->pwqs)))
		return -EINVAL;

	ctx = apply_wqattrs_prepare(wq, attrs);
	if (!ctx)
		return -ENOMEM;

	/* the ctx has been prepared successfully, let's commit it */
	apply_wqattrs_commit(ctx);
	apply_wqattrs_cleanup(ctx);

	return 0;
}