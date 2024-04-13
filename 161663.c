static void apply_wqattrs_cleanup(struct apply_wqattrs_ctx *ctx)
{
	if (ctx) {
		int node;

		for_each_node(node)
			put_pwq_unlocked(ctx->pwq_tbl[node]);
		put_pwq_unlocked(ctx->dfl_pwq);

		free_workqueue_attrs(ctx->attrs);

		kfree(ctx);
	}
}