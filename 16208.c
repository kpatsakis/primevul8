
static int io_sq_thread(void *data)
{
	struct io_sq_data *sqd = data;
	struct io_ring_ctx *ctx;
	unsigned long timeout = 0;
	char buf[TASK_COMM_LEN];
	DEFINE_WAIT(wait);

	snprintf(buf, sizeof(buf), "iou-sqp-%d", sqd->task_pid);
	set_task_comm(current, buf);

	if (sqd->sq_cpu != -1)
		set_cpus_allowed_ptr(current, cpumask_of(sqd->sq_cpu));
	else
		set_cpus_allowed_ptr(current, cpu_online_mask);
	current->flags |= PF_NO_SETAFFINITY;

	audit_alloc_kernel(current);

	mutex_lock(&sqd->lock);
	while (1) {
		bool cap_entries, sqt_spin = false;

		if (io_sqd_events_pending(sqd) || signal_pending(current)) {
			if (io_sqd_handle_event(sqd))
				break;
			timeout = jiffies + sqd->sq_thread_idle;
		}

		cap_entries = !list_is_singular(&sqd->ctx_list);
		list_for_each_entry(ctx, &sqd->ctx_list, sqd_list) {
			int ret = __io_sq_thread(ctx, cap_entries);

			if (!sqt_spin && (ret > 0 || !wq_list_empty(&ctx->iopoll_list)))
				sqt_spin = true;
		}
		if (io_run_task_work())
			sqt_spin = true;

		if (sqt_spin || !time_after(jiffies, timeout)) {
			cond_resched();
			if (sqt_spin)
				timeout = jiffies + sqd->sq_thread_idle;
			continue;
		}

		prepare_to_wait(&sqd->wait, &wait, TASK_INTERRUPTIBLE);
		if (!io_sqd_events_pending(sqd) && !task_work_pending(current)) {
			bool needs_sched = true;

			list_for_each_entry(ctx, &sqd->ctx_list, sqd_list) {
				io_ring_set_wakeup_flag(ctx);

				if ((ctx->flags & IORING_SETUP_IOPOLL) &&
				    !wq_list_empty(&ctx->iopoll_list)) {
					needs_sched = false;
					break;
				}

				/*
				 * Ensure the store of the wakeup flag is not
				 * reordered with the load of the SQ tail
				 */
				smp_mb();

				if (io_sqring_entries(ctx)) {
					needs_sched = false;
					break;
				}
			}

			if (needs_sched) {
				mutex_unlock(&sqd->lock);
				schedule();
				mutex_lock(&sqd->lock);
			}
			list_for_each_entry(ctx, &sqd->ctx_list, sqd_list)
				io_ring_clear_wakeup_flag(ctx);
		}

		finish_wait(&sqd->wait, &wait);
		timeout = jiffies + sqd->sq_thread_idle;
	}

	io_uring_cancel_generic(true, sqd);
	sqd->thread = NULL;
	list_for_each_entry(ctx, &sqd->ctx_list, sqd_list)
		io_ring_set_wakeup_flag(ctx);
	io_run_task_work();
	mutex_unlock(&sqd->lock);

	audit_free(current);

	complete(&sqd->exited);
	do_exit(0);