void kthread_unpark(struct task_struct *k)
{
	struct kthread *kthread = to_kthread(k);

	clear_bit(KTHREAD_SHOULD_PARK, &kthread->flags);
	/*
	 * We clear the IS_PARKED bit here as we don't wait
	 * until the task has left the park code. So if we'd
	 * park before that happens we'd see the IS_PARKED bit
	 * which might be about to be cleared.
	 */
	if (test_and_clear_bit(KTHREAD_IS_PARKED, &kthread->flags)) {
		/*
		 * Newly created kthread was parked when the CPU was offline.
		 * The binding was lost and we need to set it again.
		 */
		if (test_bit(KTHREAD_IS_PER_CPU, &kthread->flags))
			__kthread_bind(k, kthread->cpu, TASK_PARKED);
		wake_up_state(k, TASK_PARKED);
	}
}