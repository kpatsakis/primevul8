				struct io_rsrc_data *data_to_kill)
	__must_hold(&ctx->uring_lock)
{
	WARN_ON_ONCE(!ctx->rsrc_backup_node);
	WARN_ON_ONCE(data_to_kill && !ctx->rsrc_node);

	io_rsrc_refs_drop(ctx);

	if (data_to_kill) {
		struct io_rsrc_node *rsrc_node = ctx->rsrc_node;

		rsrc_node->rsrc_data = data_to_kill;
		spin_lock_irq(&ctx->rsrc_ref_lock);
		list_add_tail(&rsrc_node->node, &ctx->rsrc_ref_list);
		spin_unlock_irq(&ctx->rsrc_ref_lock);

		atomic_inc(&data_to_kill->refs);
		percpu_ref_kill(&rsrc_node->refs);
		ctx->rsrc_node = NULL;
	}

	if (!ctx->rsrc_node) {
		ctx->rsrc_node = ctx->rsrc_backup_node;
		ctx->rsrc_backup_node = NULL;
	}