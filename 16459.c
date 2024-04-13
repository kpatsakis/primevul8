
static int io_arm_poll_handler(struct io_kiocb *req, unsigned issue_flags)
{
	const struct io_op_def *def = &io_op_defs[req->opcode];
	struct io_ring_ctx *ctx = req->ctx;
	struct async_poll *apoll;
	struct io_poll_table ipt;
	__poll_t mask = EPOLLONESHOT | POLLERR | POLLPRI;
	int ret;

	if (!def->pollin && !def->pollout)
		return IO_APOLL_ABORTED;
	if (!file_can_poll(req->file) || (req->flags & REQ_F_POLLED))
		return IO_APOLL_ABORTED;

	if (def->pollin) {
		mask |= POLLIN | POLLRDNORM;

		/* If reading from MSG_ERRQUEUE using recvmsg, ignore POLLIN */
		if ((req->opcode == IORING_OP_RECVMSG) &&
		    (req->sr_msg.msg_flags & MSG_ERRQUEUE))
			mask &= ~POLLIN;
	} else {
		mask |= POLLOUT | POLLWRNORM;
	}
	if (def->poll_exclusive)
		mask |= EPOLLEXCLUSIVE;
	if (!(issue_flags & IO_URING_F_UNLOCKED) &&
	    !list_empty(&ctx->apoll_cache)) {
		apoll = list_first_entry(&ctx->apoll_cache, struct async_poll,
						poll.wait.entry);
		list_del_init(&apoll->poll.wait.entry);
	} else {
		apoll = kmalloc(sizeof(*apoll), GFP_ATOMIC);
		if (unlikely(!apoll))
			return IO_APOLL_ABORTED;
	}
	apoll->double_poll = NULL;
	req->apoll = apoll;
	req->flags |= REQ_F_POLLED;
	ipt.pt._qproc = io_async_queue_proc;

	io_kbuf_recycle(req, issue_flags);

	ret = __io_arm_poll_handler(req, &apoll->poll, &ipt, mask);
	if (ret || ipt.error)
		return ret ? IO_APOLL_READY : IO_APOLL_ABORTED;

	trace_io_uring_poll_arm(ctx, req, req->user_data, req->opcode,
				mask, apoll->poll.events);
	return IO_APOLL_OK;