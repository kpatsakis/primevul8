pfn_t gfn_to_pfn_async(struct kvm *kvm, gfn_t gfn, bool *async,
		       bool write_fault, bool *writable)
{
	return __gfn_to_pfn(kvm, gfn, false, async, write_fault, writable);
}