static void hardware_disable(void *junk)
{
	raw_spin_lock(&kvm_lock);
	hardware_disable_nolock(junk);
	raw_spin_unlock(&kvm_lock);
}