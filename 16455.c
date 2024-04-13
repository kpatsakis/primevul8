static void io_eventfd_signal(struct io_ring_ctx *ctx)
{
	struct io_ev_fd *ev_fd;

	rcu_read_lock();
	/*
	 * rcu_dereference ctx->io_ev_fd once and use it for both for checking
	 * and eventfd_signal
	 */
	ev_fd = rcu_dereference(ctx->io_ev_fd);

	/*
	 * Check again if ev_fd exists incase an io_eventfd_unregister call
	 * completed between the NULL check of ctx->io_ev_fd at the start of
	 * the function and rcu_read_lock.
	 */
	if (unlikely(!ev_fd))
		goto out;
	if (READ_ONCE(ctx->rings->cq_flags) & IORING_CQ_EVENTFD_DISABLED)
		goto out;

	if (!ev_fd->eventfd_async || io_wq_current_is_worker())
		eventfd_signal(ev_fd->cq_ev_fd, 1);
out:
	rcu_read_unlock();
}