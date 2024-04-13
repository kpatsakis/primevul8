
static void io_eventfd_put(struct rcu_head *rcu)
{
	struct io_ev_fd *ev_fd = container_of(rcu, struct io_ev_fd, rcu);

	eventfd_ctx_put(ev_fd->cq_ev_fd);
	kfree(ev_fd);