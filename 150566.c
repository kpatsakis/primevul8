pfn_t gfn_to_pfn_memslot(struct kvm *kvm,
			 struct kvm_memory_slot *slot, gfn_t gfn)
{
	unsigned long addr = gfn_to_hva_memslot(slot, gfn);
	return hva_to_pfn(kvm, addr, false, NULL, true, NULL);
}