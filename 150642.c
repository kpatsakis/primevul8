pfn_t gfn_to_pfn(struct kvm *kvm, gfn_t gfn)
{
	return __gfn_to_pfn(kvm, gfn, false, NULL, true, NULL);
}