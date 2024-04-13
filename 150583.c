pfn_t gfn_to_pfn_atomic(struct kvm *kvm, gfn_t gfn)
{
	return __gfn_to_pfn(kvm, gfn, true, NULL, true, NULL);
}