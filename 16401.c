static int io_epoll_ctl_prep(struct io_kiocb *req,
			     const struct io_uring_sqe *sqe)
{
#if defined(CONFIG_EPOLL)
	if (sqe->ioprio || sqe->buf_index || sqe->splice_fd_in)
		return -EINVAL;
	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;

	req->epoll.epfd = READ_ONCE(sqe->fd);
	req->epoll.op = READ_ONCE(sqe->len);
	req->epoll.fd = READ_ONCE(sqe->off);

	if (ep_op_has_event(req->epoll.op)) {
		struct epoll_event __user *ev;

		ev = u64_to_user_ptr(READ_ONCE(sqe->addr));
		if (copy_from_user(&req->epoll.event, ev, sizeof(*ev)))
			return -EFAULT;
	}

	return 0;
#else
	return -EOPNOTSUPP;
#endif
}