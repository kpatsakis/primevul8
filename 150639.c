void kvm_set_pfn_dirty(pfn_t pfn)
{
	if (!kvm_is_mmio_pfn(pfn)) {
		struct page *page = pfn_to_page(pfn);
		if (!PageReserved(page))
			SetPageDirty(page);
	}
}