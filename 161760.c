static void __kthread_parkme(struct kthread *self)
{
	__set_current_state(TASK_PARKED);
	while (test_bit(KTHREAD_SHOULD_PARK, &self->flags)) {
		if (!test_and_set_bit(KTHREAD_IS_PARKED, &self->flags))
			complete(&self->parked);
		schedule();
		__set_current_state(TASK_PARKED);
	}
	clear_bit(KTHREAD_IS_PARKED, &self->flags);
	__set_current_state(TASK_RUNNING);
}