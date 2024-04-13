
static int __io_uring_add_tctx_node(struct io_ring_ctx *ctx)
{
	struct io_uring_task *tctx = current->io_uring;
	struct io_tctx_node *node;
	int ret;

	if (unlikely(!tctx)) {
		ret = io_uring_alloc_task_context(current, ctx);
		if (unlikely(ret))
			return ret;

		tctx = current->io_uring;
		if (ctx->iowq_limits_set) {
			unsigned int limits[2] = { ctx->iowq_limits[0],
						   ctx->iowq_limits[1], };

			ret = io_wq_max_workers(tctx->io_wq, limits);
			if (ret)
				return ret;
		}
	}
	if (!xa_load(&tctx->xa, (unsigned long)ctx)) {
		node = kmalloc(sizeof(*node), GFP_KERNEL);
		if (!node)
			return -ENOMEM;
		node->ctx = ctx;
		node->task = current;

		ret = xa_err(xa_store(&tctx->xa, (unsigned long)ctx,
					node, GFP_KERNEL));
		if (ret) {
			kfree(node);
			return ret;
		}

		mutex_lock(&ctx->uring_lock);
		list_add(&node->ctx_node, &ctx->tctx_list);
		mutex_unlock(&ctx->uring_lock);
	}
	tctx->last = ctx;
	return 0;