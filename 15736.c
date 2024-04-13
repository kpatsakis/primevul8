	__must_hold(&ctx->uring_lock)
{
	struct io_cancel_data cd = {
		.ctx	= ctx,
		.seq	= atomic_inc_return(&ctx->cancel_seq),
	};
	ktime_t timeout = KTIME_MAX;
	struct io_uring_sync_cancel_reg sc;
	struct fd f = { };
	DEFINE_WAIT(wait);
	int ret;

	if (copy_from_user(&sc, arg, sizeof(sc)))
		return -EFAULT;
	if (sc.flags & ~CANCEL_FLAGS)
		return -EINVAL;
	if (sc.pad[0] || sc.pad[1] || sc.pad[2] || sc.pad[3])
		return -EINVAL;

	cd.data = sc.addr;
	cd.flags = sc.flags;

	/* we can grab a normal file descriptor upfront */
	if ((cd.flags & IORING_ASYNC_CANCEL_FD) &&
	   !(cd.flags & IORING_ASYNC_CANCEL_FD_FIXED)) {
		f = fdget(sc.fd);
		if (!f.file)
			return -EBADF;
		cd.file = f.file;
	}

	ret = __io_sync_cancel(current->io_uring, &cd, sc.fd);

	/* found something, done! */
	if (ret != -EALREADY)
		goto out;

	if (sc.timeout.tv_sec != -1UL || sc.timeout.tv_nsec != -1UL) {
		struct timespec64 ts = {
			.tv_sec		= sc.timeout.tv_sec,
			.tv_nsec	= sc.timeout.tv_nsec
		};

		timeout = ktime_add_ns(timespec64_to_ktime(ts), ktime_get_ns());
	}

	/*
	 * Keep looking until we get -ENOENT. we'll get woken everytime
	 * every time a request completes and will retry the cancelation.
	 */
	do {
		cd.seq = atomic_inc_return(&ctx->cancel_seq);

		prepare_to_wait(&ctx->cq_wait, &wait, TASK_INTERRUPTIBLE);

		ret = __io_sync_cancel(current->io_uring, &cd, sc.fd);

		if (ret != -EALREADY)
			break;

		mutex_unlock(&ctx->uring_lock);
		ret = io_run_task_work_sig();
		if (ret < 0) {
			mutex_lock(&ctx->uring_lock);
			break;
		}
		ret = schedule_hrtimeout(&timeout, HRTIMER_MODE_ABS);
		mutex_lock(&ctx->uring_lock);
		if (!ret) {
			ret = -ETIME;
			break;
		}
	} while (1);

	finish_wait(&ctx->cq_wait, &wait);

	if (ret == -ENOENT || ret > 0)
		ret = 0;
out:
	fdput(f);
	return ret;
}