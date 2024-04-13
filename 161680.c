int kthread_stop(struct task_struct *k)
{
	struct kthread *kthread;
	int ret;

	trace_sched_kthread_stop(k);

	get_task_struct(k);
	kthread = to_kthread(k);
	set_bit(KTHREAD_SHOULD_STOP, &kthread->flags);
	kthread_unpark(k);
	wake_up_process(k);
	wait_for_completion(&kthread->exited);
	ret = k->exit_code;
	put_task_struct(k);

	trace_sched_kthread_stop_ret(ret);
	return ret;
}