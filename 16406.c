	__must_hold(&ctx->uring_lock)
{
	struct task_struct *task = NULL;
	int task_refs = 0;

	do {
		struct io_kiocb *req = container_of(node, struct io_kiocb,
						    comp_list);

		if (unlikely(req->flags & REQ_F_REFCOUNT)) {
			node = req->comp_list.next;
			if (!req_ref_put_and_test(req))
				continue;
		}

		io_req_put_rsrc_locked(req, ctx);
		io_queue_next(req);
		io_dismantle_req(req);

		if (req->task != task) {
			if (task)
				io_put_task(task, task_refs);
			task = req->task;
			task_refs = 0;
		}
		task_refs++;
		node = req->comp_list.next;
		wq_stack_add_head(&req->comp_list, &ctx->submit_state.free_list);
	} while (node);

	if (task)
		io_put_task(task, task_refs);
}