pfn_t gfn_to_pfn_prot(struct kvm *kvm, gfn_t gfn, bool write_fault,
		      bool *writable)
{
	return __gfn_to_pfn(kvm, gfn, false, NULL, write_fault, writable);
}