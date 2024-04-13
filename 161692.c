bool kthread_should_stop(void)
{
	return test_bit(KTHREAD_SHOULD_STOP, &to_kthread(current)->flags);
}