static void apply_wqattrs_commit(struct apply_wqattrs_ctx *ctx)
{
	int node;

	/* all pwqs have been created successfully, let's install'em */
	mutex_lock(&ctx->wq->mutex);

	copy_workqueue_attrs(ctx->wq->unbound_attrs, ctx->attrs);

	/* save the previous pwq and install the new one */
	for_each_node(node)
		ctx->pwq_tbl[node] = numa_pwq_tbl_install(ctx->wq, node,
							  ctx->pwq_tbl[node]);

	/* @dfl_pwq might not have been used, ensure it's linked */
	link_pwq(ctx->dfl_pwq);
	swap(ctx->wq->dfl_pwq, ctx->dfl_pwq);

	mutex_unlock(&ctx->wq->mutex);
}