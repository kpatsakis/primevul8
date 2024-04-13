 */
static int io_cqring_wait(struct io_ring_ctx *ctx, int min_events,
			  const sigset_t __user *sig, size_t sigsz,
			  struct __kernel_timespec __user *uts)
{
	struct io_wait_queue iowq;
	struct io_rings *rings = ctx->rings;
	ktime_t timeout = KTIME_MAX;
	int ret;

	do {
		io_cqring_overflow_flush(ctx);
		if (io_cqring_events(ctx) >= min_events)
			return 0;
		if (!io_run_task_work())
			break;
	} while (1);

	if (sig) {
#ifdef CONFIG_COMPAT
		if (in_compat_syscall())
			ret = set_compat_user_sigmask((const compat_sigset_t __user *)sig,
						      sigsz);
		else
#endif
			ret = set_user_sigmask(sig, sigsz);

		if (ret)
			return ret;
	}

	if (uts) {
		struct timespec64 ts;

		if (get_timespec64(&ts, uts))
			return -EFAULT;
		timeout = ktime_add_ns(timespec64_to_ktime(ts), ktime_get_ns());
	}

	init_waitqueue_func_entry(&iowq.wq, io_wake_function);
	iowq.wq.private = current;
	INIT_LIST_HEAD(&iowq.wq.entry);
	iowq.ctx = ctx;
	iowq.nr_timeouts = atomic_read(&ctx->cq_timeouts);
	iowq.cq_tail = READ_ONCE(ctx->rings->cq.head) + min_events;

	trace_io_uring_cqring_wait(ctx, min_events);
	do {
		/* if we can't even flush overflow, don't wait for more */
		if (!io_cqring_overflow_flush(ctx)) {
			ret = -EBUSY;
			break;
		}
		prepare_to_wait_exclusive(&ctx->cq_wait, &iowq.wq,
						TASK_INTERRUPTIBLE);
		ret = io_cqring_wait_schedule(ctx, &iowq, timeout);
		finish_wait(&ctx->cq_wait, &iowq.wq);
		cond_resched();
	} while (ret > 0);

	restore_saved_sigmask_unless(ret == -EINTR);

	return READ_ONCE(rings->cq.head) == READ_ONCE(rings->cq.tail) ? ret : 0;