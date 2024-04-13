pfn_t hva_to_pfn_atomic(struct kvm *kvm, unsigned long addr)
{
	return hva_to_pfn(kvm, addr, true, NULL, true, NULL);
}