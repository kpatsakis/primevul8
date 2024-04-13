	__must_hold(&ctx->uring_lock)
{
	struct io_wq_work_node *node, *prev;
	struct io_submit_state *state = &ctx->submit_state;

	if (state->flush_cqes) {
		spin_lock(&ctx->completion_lock);
		wq_list_for_each(node, prev, &state->compl_reqs) {
			struct io_kiocb *req = container_of(node, struct io_kiocb,
						    comp_list);

			if (!(req->flags & REQ_F_CQE_SKIP))
				__io_fill_cqe_req(req, req->result, req->cflags);
			if ((req->flags & REQ_F_POLLED) && req->apoll) {
				struct async_poll *apoll = req->apoll;

				if (apoll->double_poll)
					kfree(apoll->double_poll);
				list_add(&apoll->poll.wait.entry,
						&ctx->apoll_cache);
				req->flags &= ~REQ_F_POLLED;
			}
		}

		io_commit_cqring(ctx);
		spin_unlock(&ctx->completion_lock);
		io_cqring_ev_posted(ctx);
		state->flush_cqes = false;
	}

	io_free_batch_list(ctx, state->compl_reqs.first);
	INIT_WQ_LIST(&state->compl_reqs);
}