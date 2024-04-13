bool kthread_should_park(void)
{
	return test_bit(KTHREAD_SHOULD_PARK, &to_kthread(current)->flags);
}