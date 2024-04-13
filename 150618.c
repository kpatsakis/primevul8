static void hardware_enable(void *junk)
{
	raw_spin_lock(&kvm_lock);
	hardware_enable_nolock(junk);
	raw_spin_unlock(&kvm_lock);
}