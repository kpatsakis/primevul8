struct kvm_memory_slot *gfn_to_memslot(struct kvm *kvm, gfn_t gfn)
{
	return __gfn_to_memslot(kvm_memslots(kvm), gfn);
}