int kthread_park(struct task_struct *k)
{
	struct kthread *kthread = to_kthread(k);

	if (WARN_ON(k->flags & PF_EXITING))
		return -ENOSYS;

	if (!test_bit(KTHREAD_IS_PARKED, &kthread->flags)) {
		set_bit(KTHREAD_SHOULD_PARK, &kthread->flags);
		if (k != current) {
			wake_up_process(k);
			wait_for_completion(&kthread->parked);
		}
	}

	return 0;
}