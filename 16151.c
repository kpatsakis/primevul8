static void handle_tw_list(struct io_wq_work_node *node,
			   struct io_ring_ctx **ctx, bool *locked)
{
	do {
		struct io_wq_work_node *next = node->next;
		struct io_kiocb *req = container_of(node, struct io_kiocb,
						    io_task_work.node);

		prefetch(container_of(next, struct io_kiocb, io_task_work.node));

		if (req->ctx != *ctx) {
			ctx_flush_and_put(*ctx, locked);
			*ctx = req->ctx;
			/* if not contended, grab and improve batching */
			*locked = mutex_trylock(&(*ctx)->uring_lock);
			percpu_ref_get(&(*ctx)->refs);
		}
		req->io_task_work.func(req, locked);
		node = next;
	} while (node);
}