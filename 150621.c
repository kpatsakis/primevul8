static void kvm_resume(void)
{
	if (kvm_usage_count) {
		WARN_ON(raw_spin_is_locked(&kvm_lock));
		hardware_enable_nolock(NULL);
	}
}