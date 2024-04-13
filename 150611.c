static void hardware_disable_all(void)
{
	raw_spin_lock(&kvm_lock);
	hardware_disable_all_nolock();
	raw_spin_unlock(&kvm_lock);
}