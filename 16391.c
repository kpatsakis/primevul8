static int io_do_iopoll(struct io_ring_ctx *ctx, bool force_nonspin)
{
	struct io_wq_work_node *pos, *start, *prev;
	unsigned int poll_flags = BLK_POLL_NOSLEEP;
	DEFINE_IO_COMP_BATCH(iob);
	int nr_events = 0;

	/*
	 * Only spin for completions if we don't have multiple devices hanging
	 * off our complete list.
	 */
	if (ctx->poll_multi_queue || force_nonspin)
		poll_flags |= BLK_POLL_ONESHOT;

	wq_list_for_each(pos, start, &ctx->iopoll_list) {
		struct io_kiocb *req = container_of(pos, struct io_kiocb, comp_list);
		struct kiocb *kiocb = &req->rw.kiocb;
		int ret;

		/*
		 * Move completed and retryable entries to our local lists.
		 * If we find a request that requires polling, break out
		 * and complete those lists first, if we have entries there.
		 */
		if (READ_ONCE(req->iopoll_completed))
			break;

		ret = kiocb->ki_filp->f_op->iopoll(kiocb, &iob, poll_flags);
		if (unlikely(ret < 0))
			return ret;
		else if (ret)
			poll_flags |= BLK_POLL_ONESHOT;

		/* iopoll may have completed current req */
		if (!rq_list_empty(iob.req_list) ||
		    READ_ONCE(req->iopoll_completed))
			break;
	}

	if (!rq_list_empty(iob.req_list))
		iob.complete(&iob);
	else if (!pos)
		return 0;

	prev = start;
	wq_list_for_each_resume(pos, prev) {
		struct io_kiocb *req = container_of(pos, struct io_kiocb, comp_list);

		/* order with io_complete_rw_iopoll(), e.g. ->result updates */
		if (!smp_load_acquire(&req->iopoll_completed))
			break;
		nr_events++;
		if (unlikely(req->flags & REQ_F_CQE_SKIP))
			continue;
		__io_fill_cqe_req(req, req->result, io_put_kbuf(req, 0));
	}

	if (unlikely(!nr_events))
		return 0;

	io_commit_cqring(ctx);
	io_cqring_ev_posted_iopoll(ctx);
	pos = start ? start->next : ctx->iopoll_list.first;
	wq_list_cut(&ctx->iopoll_list, prev, start);
	io_free_batch_list(ctx, pos);
	return nr_events;
}