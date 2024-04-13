static inline bool io_run_task_work(void)
{
	if (test_thread_flag(TIF_NOTIFY_SIGNAL) || task_work_pending(current)) {
		__set_current_state(TASK_RUNNING);
		clear_notify_signal();
		if (task_work_pending(current))
			task_work_run();
		return true;
	}

	return false;
}