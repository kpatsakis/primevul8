void kthread_parkme(void)
{
	__kthread_parkme(to_kthread(current));
}