int kvm_set_memory_region(struct kvm *kvm,
			  struct kvm_userspace_memory_region *mem,
			  int user_alloc)
{
	int r;

	mutex_lock(&kvm->slots_lock);
	r = __kvm_set_memory_region(kvm, mem, user_alloc);
	mutex_unlock(&kvm->slots_lock);
	return r;
}