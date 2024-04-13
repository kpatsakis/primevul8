static int kvm_suspend(void)
{
	if (kvm_usage_count)
		hardware_disable_nolock(NULL);
	return 0;
}