
static int io_eventfd_unregister(struct io_ring_ctx *ctx)
{
	struct io_ev_fd *ev_fd;

	ev_fd = rcu_dereference_protected(ctx->io_ev_fd,
					lockdep_is_held(&ctx->uring_lock));
	if (ev_fd) {
		ctx->has_evfd = false;
		rcu_assign_pointer(ctx->io_ev_fd, NULL);
		call_rcu(&ev_fd->rcu, io_eventfd_put);
		return 0;
	}

	return -ENXIO;