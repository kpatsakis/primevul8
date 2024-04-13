void kvm_get_kvm(struct kvm *kvm)
{
	atomic_inc(&kvm->users_count);
}