
static void io_poll_remove_entries(struct io_kiocb *req)
{
	/*
	 * Nothing to do if neither of those flags are set. Avoid dipping
	 * into the poll/apoll/double cachelines if we can.
	 */
	if (!(req->flags & (REQ_F_SINGLE_POLL | REQ_F_DOUBLE_POLL)))
		return;

	/*
	 * While we hold the waitqueue lock and the waitqueue is nonempty,
	 * wake_up_pollfree() will wait for us.  However, taking the waitqueue
	 * lock in the first place can race with the waitqueue being freed.
	 *
	 * We solve this as eventpoll does: by taking advantage of the fact that
	 * all users of wake_up_pollfree() will RCU-delay the actual free.  If
	 * we enter rcu_read_lock() and see that the pointer to the queue is
	 * non-NULL, we can then lock it without the memory being freed out from
	 * under us.
	 *
	 * Keep holding rcu_read_lock() as long as we hold the queue lock, in
	 * case the caller deletes the entry from the queue, leaving it empty.
	 * In that case, only RCU prevents the queue memory from being freed.
	 */
	rcu_read_lock();
	if (req->flags & REQ_F_SINGLE_POLL)
		io_poll_remove_entry(io_poll_get_single(req));
	if (req->flags & REQ_F_DOUBLE_POLL)
		io_poll_remove_entry(io_poll_get_double(req));
	rcu_read_unlock();