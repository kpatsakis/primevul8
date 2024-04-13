
static void __io_queue_proc(struct io_poll_iocb *poll, struct io_poll_table *pt,
			    struct wait_queue_head *head,
			    struct io_poll_iocb **poll_ptr)
{
	struct io_kiocb *req = pt->req;
	unsigned long wqe_private = (unsigned long) req;

	/*
	 * The file being polled uses multiple waitqueues for poll handling
	 * (e.g. one for read, one for write). Setup a separate io_poll_iocb
	 * if this happens.
	 */
	if (unlikely(pt->nr_entries)) {
		struct io_poll_iocb *first = poll;

		/* double add on the same waitqueue head, ignore */
		if (first->head == head)
			return;
		/* already have a 2nd entry, fail a third attempt */
		if (*poll_ptr) {
			if ((*poll_ptr)->head == head)
				return;
			pt->error = -EINVAL;
			return;
		}

		poll = kmalloc(sizeof(*poll), GFP_ATOMIC);
		if (!poll) {
			pt->error = -ENOMEM;
			return;
		}
		/* mark as double wq entry */
		wqe_private |= 1;
		req->flags |= REQ_F_DOUBLE_POLL;
		io_init_poll_iocb(poll, first->events, first->wait.func);
		*poll_ptr = poll;
		if (req->opcode == IORING_OP_POLL_ADD)
			req->flags |= REQ_F_ASYNC_DATA;
	}

	req->flags |= REQ_F_SINGLE_POLL;
	pt->nr_entries++;
	poll->head = head;
	poll->wait.private = (void *) wqe_private;

	if (poll->events & EPOLLEXCLUSIVE)
		add_wait_queue_exclusive(head, &poll->wait);
	else
		add_wait_queue(head, &poll->wait);