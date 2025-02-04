
static __cold void __io_uring_show_fdinfo(struct io_ring_ctx *ctx,
					  struct seq_file *m)
{
	struct io_sq_data *sq = NULL;
	struct io_overflow_cqe *ocqe;
	struct io_rings *r = ctx->rings;
	unsigned int sq_mask = ctx->sq_entries - 1, cq_mask = ctx->cq_entries - 1;
	unsigned int sq_head = READ_ONCE(r->sq.head);
	unsigned int sq_tail = READ_ONCE(r->sq.tail);
	unsigned int cq_head = READ_ONCE(r->cq.head);
	unsigned int cq_tail = READ_ONCE(r->cq.tail);
	unsigned int sq_entries, cq_entries;
	bool has_lock;
	unsigned int i;

	/*
	 * we may get imprecise sqe and cqe info if uring is actively running
	 * since we get cached_sq_head and cached_cq_tail without uring_lock
	 * and sq_tail and cq_head are changed by userspace. But it's ok since
	 * we usually use these info when it is stuck.
	 */
	seq_printf(m, "SqMask:\t0x%x\n", sq_mask);
	seq_printf(m, "SqHead:\t%u\n", sq_head);
	seq_printf(m, "SqTail:\t%u\n", sq_tail);
	seq_printf(m, "CachedSqHead:\t%u\n", ctx->cached_sq_head);
	seq_printf(m, "CqMask:\t0x%x\n", cq_mask);
	seq_printf(m, "CqHead:\t%u\n", cq_head);
	seq_printf(m, "CqTail:\t%u\n", cq_tail);
	seq_printf(m, "CachedCqTail:\t%u\n", ctx->cached_cq_tail);
	seq_printf(m, "SQEs:\t%u\n", sq_tail - ctx->cached_sq_head);
	sq_entries = min(sq_tail - sq_head, ctx->sq_entries);
	for (i = 0; i < sq_entries; i++) {
		unsigned int entry = i + sq_head;
		unsigned int sq_idx = READ_ONCE(ctx->sq_array[entry & sq_mask]);
		struct io_uring_sqe *sqe;

		if (sq_idx > sq_mask)
			continue;
		sqe = &ctx->sq_sqes[sq_idx];
		seq_printf(m, "%5u: opcode:%d, fd:%d, flags:%x, user_data:%llu\n",
			   sq_idx, sqe->opcode, sqe->fd, sqe->flags,
			   sqe->user_data);
	}
	seq_printf(m, "CQEs:\t%u\n", cq_tail - cq_head);
	cq_entries = min(cq_tail - cq_head, ctx->cq_entries);
	for (i = 0; i < cq_entries; i++) {
		unsigned int entry = i + cq_head;
		struct io_uring_cqe *cqe = &r->cqes[entry & cq_mask];

		seq_printf(m, "%5u: user_data:%llu, res:%d, flag:%x\n",
			   entry & cq_mask, cqe->user_data, cqe->res,
			   cqe->flags);
	}

	/*
	 * Avoid ABBA deadlock between the seq lock and the io_uring mutex,
	 * since fdinfo case grabs it in the opposite direction of normal use
	 * cases. If we fail to get the lock, we just don't iterate any
	 * structures that could be going away outside the io_uring mutex.
	 */
	has_lock = mutex_trylock(&ctx->uring_lock);

	if (has_lock && (ctx->flags & IORING_SETUP_SQPOLL)) {
		sq = ctx->sq_data;
		if (!sq->thread)
			sq = NULL;
	}

	seq_printf(m, "SqThread:\t%d\n", sq ? task_pid_nr(sq->thread) : -1);
	seq_printf(m, "SqThreadCpu:\t%d\n", sq ? task_cpu(sq->thread) : -1);
	seq_printf(m, "UserFiles:\t%u\n", ctx->nr_user_files);
	for (i = 0; has_lock && i < ctx->nr_user_files; i++) {
		struct file *f = io_file_from_index(ctx, i);

		if (f)
			seq_printf(m, "%5u: %s\n", i, file_dentry(f)->d_iname);
		else
			seq_printf(m, "%5u: <none>\n", i);
	}
	seq_printf(m, "UserBufs:\t%u\n", ctx->nr_user_bufs);
	for (i = 0; has_lock && i < ctx->nr_user_bufs; i++) {
		struct io_mapped_ubuf *buf = ctx->user_bufs[i];
		unsigned int len = buf->ubuf_end - buf->ubuf;

		seq_printf(m, "%5u: 0x%llx/%u\n", i, buf->ubuf, len);
	}
	if (has_lock && !xa_empty(&ctx->personalities)) {
		unsigned long index;
		const struct cred *cred;

		seq_printf(m, "Personalities:\n");
		xa_for_each(&ctx->personalities, index, cred)
			io_uring_show_cred(m, index, cred);
	}
	if (has_lock)
		mutex_unlock(&ctx->uring_lock);

	seq_puts(m, "PollList:\n");
	spin_lock(&ctx->completion_lock);
	for (i = 0; i < (1U << ctx->cancel_hash_bits); i++) {
		struct hlist_head *list = &ctx->cancel_hash[i];
		struct io_kiocb *req;

		hlist_for_each_entry(req, list, hash_node)
			seq_printf(m, "  op=%d, task_works=%d\n", req->opcode,
					task_work_pending(req->task));
	}

	seq_puts(m, "CqOverflowList:\n");
	list_for_each_entry(ocqe, &ctx->cq_overflow_list, list) {
		struct io_uring_cqe *cqe = &ocqe->cqe;

		seq_printf(m, "  user_data=%llu, res=%d, flags=%x\n",
			   cqe->user_data, cqe->res, cqe->flags);

	}

	spin_unlock(&ctx->completion_lock);