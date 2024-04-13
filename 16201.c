static void handle_prev_tw_list(struct io_wq_work_node *node,
				struct io_ring_ctx **ctx, bool *uring_locked)
{
	if (*ctx && !*uring_locked)
		spin_lock(&(*ctx)->completion_lock);

	do {
		struct io_wq_work_node *next = node->next;
		struct io_kiocb *req = container_of(node, struct io_kiocb,
						    io_task_work.node);

		prefetch(container_of(next, struct io_kiocb, io_task_work.node));

		if (req->ctx != *ctx) {
			if (unlikely(!*uring_locked && *ctx))
				ctx_commit_and_unlock(*ctx);

			ctx_flush_and_put(*ctx, uring_locked);
			*ctx = req->ctx;
			/* if not contended, grab and improve batching */
			*uring_locked = mutex_trylock(&(*ctx)->uring_lock);
			percpu_ref_get(&(*ctx)->refs);
			if (unlikely(!*uring_locked))
				spin_lock(&(*ctx)->completion_lock);
		}
		if (likely(*uring_locked))
			req->io_task_work.func(req, uring_locked);
		else
			__io_req_complete_post(req, req->result,
						io_put_kbuf_comp(req));
		node = next;
	} while (node);

	if (unlikely(!*uring_locked))
		ctx_commit_and_unlock(*ctx);
}