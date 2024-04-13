
static int io_eventfd_register(struct io_ring_ctx *ctx, void __user *arg,
			       unsigned int eventfd_async)
{
	struct io_ev_fd *ev_fd;
	__s32 __user *fds = arg;
	int fd;

	ev_fd = rcu_dereference_protected(ctx->io_ev_fd,
					lockdep_is_held(&ctx->uring_lock));
	if (ev_fd)
		return -EBUSY;

	if (copy_from_user(&fd, fds, sizeof(*fds)))
		return -EFAULT;

	ev_fd = kmalloc(sizeof(*ev_fd), GFP_KERNEL);
	if (!ev_fd)
		return -ENOMEM;

	ev_fd->cq_ev_fd = eventfd_ctx_fdget(fd);
	if (IS_ERR(ev_fd->cq_ev_fd)) {
		int ret = PTR_ERR(ev_fd->cq_ev_fd);
		kfree(ev_fd);
		return ret;
	}
	ev_fd->eventfd_async = eventfd_async;
	ctx->has_evfd = true;
	rcu_assign_pointer(ctx->io_ev_fd, ev_fd);
	return 0;