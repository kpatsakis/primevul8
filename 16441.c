
static int io_poll_wake(struct wait_queue_entry *wait, unsigned mode, int sync,
			void *key)
{
	struct io_kiocb *req = wqe_to_req(wait);
	struct io_poll_iocb *poll = container_of(wait, struct io_poll_iocb,
						 wait);
	__poll_t mask = key_to_poll(key);

	if (unlikely(mask & POLLFREE)) {
		io_poll_mark_cancelled(req);
		/* we have to kick tw in case it's not already */
		io_poll_execute(req, 0, poll->events);

		/*
		 * If the waitqueue is being freed early but someone is already
		 * holds ownership over it, we have to tear down the request as
		 * best we can. That means immediately removing the request from
		 * its waitqueue and preventing all further accesses to the
		 * waitqueue via the request.
		 */
		list_del_init(&poll->wait.entry);

		/*
		 * Careful: this *must* be the last step, since as soon
		 * as req->head is NULL'ed out, the request can be
		 * completed and freed, since aio_poll_complete_work()
		 * will no longer need to take the waitqueue lock.
		 */
		smp_store_release(&poll->head, NULL);
		return 1;
	}

	/* for instances that support it check for an event match first */
	if (mask && !(mask & poll->events))
		return 0;

	if (io_poll_get_ownership(req)) {
		/* optional, saves extra locking for removal in tw handler */
		if (mask && poll->events & EPOLLONESHOT) {
			list_del_init(&poll->wait.entry);
			poll->head = NULL;
			if (wqe_is_double(wait))
				req->flags &= ~REQ_F_DOUBLE_POLL;
			else
				req->flags &= ~REQ_F_SINGLE_POLL;
		}
		__io_poll_execute(req, mask, poll->events);
	}
	return 1;