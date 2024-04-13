static pfn_t __gfn_to_pfn(struct kvm *kvm, gfn_t gfn, bool atomic, bool *async,
			  bool write_fault, bool *writable)
{
	unsigned long addr;

	if (async)
		*async = false;

	addr = gfn_to_hva(kvm, gfn);
	if (kvm_is_error_hva(addr)) {
		get_page(bad_page);
		return page_to_pfn(bad_page);
	}

	return hva_to_pfn(kvm, addr, atomic, async, write_fault, writable);
}