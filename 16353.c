
static __cold void io_rsrc_node_ref_zero(struct percpu_ref *ref)
{
	struct io_rsrc_node *node = container_of(ref, struct io_rsrc_node, refs);
	struct io_ring_ctx *ctx = node->rsrc_data->ctx;
	unsigned long flags;
	bool first_add = false;
	unsigned long delay = HZ;

	spin_lock_irqsave(&ctx->rsrc_ref_lock, flags);
	node->done = true;

	/* if we are mid-quiesce then do not delay */
	if (node->rsrc_data->quiesce)
		delay = 0;

	while (!list_empty(&ctx->rsrc_ref_list)) {
		node = list_first_entry(&ctx->rsrc_ref_list,
					    struct io_rsrc_node, node);
		/* recycle ref nodes in order */
		if (!node->done)
			break;
		list_del(&node->node);
		first_add |= llist_add(&node->llist, &ctx->rsrc_put_llist);
	}
	spin_unlock_irqrestore(&ctx->rsrc_ref_lock, flags);

	if (first_add)
		mod_delayed_work(system_wq, &ctx->rsrc_put_work, delay);